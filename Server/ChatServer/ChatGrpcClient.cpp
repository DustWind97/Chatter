#include "ChatGrpcClient.h"
#include "RedisMgr.h"
#include "ConfigMgr.h"
#include "UserMgr.h"
#include "CSession.h"
#include "MysqlMgr.h"

//连接池有参构造
ChatConPool::ChatConPool(size_t poolSize, std::string host, std::string port): 
	poolSize_(poolSize),
	host_(host),
	port_(port),
	b_stop_(false)
{
	//遍历连接池大小，创建对应数量的连接
	for (size_t i = 0; i < poolSize_; ++i) 
	{
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port, grpc::InsecureChannelCredentials());//创建连接管道
		connections_.push(ChatService::NewStub(channel));//创建哨兵（邮递员发信的作用）//右值
	}
}

//连接池析构
ChatConPool::~ChatConPool()
{
	std::lock_guard<std::mutex> lock(mutex_);
	Close();
	while (!connections_.empty()) {
		connections_.pop();
	}
}

//获取连接
std::unique_ptr<ChatService::Stub> ChatConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	//等待锁
	cond_.wait(lock, [this] {
		if (b_stop_) 
		{
			return true;
		}
		return !connections_.empty();
		});
	//如果停止则直接返回空指针
	if (b_stop_) {
		return  nullptr;
	}
	std::unique_ptr<ChatService::Stub> context = std::move(connections_.front());
	connections_.pop();
	return context;
}

//返回连接
void ChatConPool::returnConnection(std::unique_ptr<ChatService::Stub> context)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) 
	{
		return;
	}
	connections_.push(std::move(context));
	cond_.notify_one();
}

//关闭连接
void ChatConPool::Close()
{
	b_stop_ = true;
	cond_.notify_all();
}

/***************************************************************************************************/
//构造
ChatGrpcClient::ChatGrpcClient()
{
	auto& cfg = ConfigMgr::Inst();
	std::string server_list = cfg["PeerServer"]["Servers"];

	std::vector<std::string> servers;
	std::stringstream sl(server_list);
	std::string server;

	//由于对端服务器可能有多个，PeerServer字符串类型的服务器组，用逗号分隔的(注意是英文半角逗号)
	//将这一组服务器解析出来
	while (std::getline(sl, server, ','))
	{
		servers.push_back(server);
	}

	//遍历服务器组
	for (std::string server: servers)
	{
		if (cfg[server]["Name"].empty())
		{
			continue;
		}
		_pools[cfg[server]["Name"]] = std::make_unique<ChatConPool>(5, cfg[server]["Host"], cfg[server]["Port"]);
	}
}

//析构
ChatGrpcClient::~ChatGrpcClient()
{
}

//获取基本信息
bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	//优先查redis中查询用户信息
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base) {
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		userinfo->uid = root["uid"].asInt();
		userinfo->name = root["name"].asString();
		userinfo->pwd = root["pwd"].asString();
		userinfo->email = root["email"].asString();
		userinfo->nick = root["nick"].asString();
		userinfo->desc = root["desc"].asString();
		userinfo->sex = root["sex"].asInt();
		userinfo->icon = root["icon"].asString();
		std::cout << "user login uid is  " << userinfo->uid << " name  is "
			<< userinfo->name << " pwd is " << userinfo->pwd << " email is " 
			<< userinfo->email << std::endl;
	}
	else {
		//redis中没有则查询mysql
		//查询数据库
		std::shared_ptr<UserInfo> user_info = nullptr;
		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if (user_info == nullptr) {
			return false;
		}

		userinfo = user_info;

		//将数据库内容写入redis缓存
		Json::Value redis_root;
		redis_root["uid"] = uid;
		redis_root["pwd"] = userinfo->pwd;
		redis_root["name"] = userinfo->name;
		redis_root["email"] = userinfo->email;
		redis_root["nick"] = userinfo->nick;
		redis_root["desc"] = userinfo->desc;
		redis_root["sex"] = userinfo->sex;
		redis_root["icon"] = userinfo->icon;
		RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
	}
	return true;
}

//通知添加好友	发送处理
AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req)
{
	AddFriendRsp rsp;
	rsp.set_error(ErrorCodes::SUCCESS); //默认发送成功码
	Defer defer([&rsp, &req]() {
		//rsp.set_error(ErrorCodes::SUCCESS); //默认发送成功码
		rsp.set_applyuid(req.applyuid()); //设置申请人ID
		rsp.set_touid(req.touid());//设置对方ID
		});

	//查询
	auto find_iter = _pools.find(server_ip);
	if (find_iter == _pools.end())//没查到对方服务器
	{
		return rsp;
	}
	//查到对面服务器
	auto& pool = find_iter->second;
	ClientContext context;
	auto stub = pool->getConnection();
	Status status = stub->NotifyAddFriend(&context, req, &rsp);//调用grpc服务转发给另一个服务器	
	Defer defercon([&stub, this, &pool]() {
		pool->returnConnection(std::move(stub));
		});

	if (!status.ok()) {
		rsp.set_error(ErrorCodes::RPCFailed);
		return rsp;
	}  

	return rsp;
}

//通知认证好友
AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req)
{
	std::cout << "-------GRPC Client-------" << std::endl;
	AuthFriendRsp rsp;
	rsp.set_error(ErrorCodes::SUCCESS);

	Defer defer([&rsp, &req]() {
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid());
		});

	auto find_iter = _pools.find(server_ip);
	if (find_iter == _pools.end()) 
	{
		return rsp;
	}

	auto& pool = find_iter->second;
	ClientContext context;
	auto stub = pool->getConnection();
	Status status = stub->NotifyAuthFriend(&context, req, &rsp);//调用grpc服务转发给另一个服务器
	Defer defercon([&stub, this, &pool]() {
		pool->returnConnection(std::move(stub));
		});

	if (!status.ok())
	{
		rsp.set_error(ErrorCodes::RPCFailed);
		return rsp;
	}

	return rsp;
}

//通知文本聊天消息
TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue)
{
	TextChatMsgRsp rsp;
	rsp.set_error(ErrorCodes::SUCCESS);

	Defer defer([&rsp, &req]() {
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid());
		for (const auto& text_data : req.textmsgs()) {
			TextChatData* new_msg = rsp.add_textmsgs();
			new_msg->set_msgid(text_data.msgid());
			new_msg->set_msgcontent(text_data.msgcontent());
		}

		});

	//在连接池中查找服务器ip
	auto find_iter = _pools.find(server_ip);
	//没找到
	if (find_iter == _pools.end()) {
		return rsp;
	}

	//找到
	auto& pool = find_iter->second;
	ClientContext context;
	auto stub = pool->getConnection();
	Status status = stub->NotifyTextChatMsg(&context, req, &rsp);
	Defer defercon([&stub, this, &pool]() {
		pool->returnConnection(std::move(stub));
		});

	if (!status.ok()) {
		rsp.set_error(ErrorCodes::RPCFailed);
		return rsp;
	}

	return rsp;
}


