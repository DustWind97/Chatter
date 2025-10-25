#include "LogicSystem.h"
#include "StatusGrpcClient.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "UserMgr.h"
#include "ChatGrpcClient.h"

//构造函数
LogicSystem::LogicSystem() :_b_stop(false) 
{
    RegisterCallBacks();
    _worker_thread = std::thread(&LogicSystem::DealMsg, this);
}

//析构函数
LogicSystem::~LogicSystem()
{
    _b_stop = true;
    _consume.notify_one();
    _worker_thread.join();
}

//处理消息
void LogicSystem::DealMsg()
{
	for (;;) {
		std::unique_lock<std::mutex> unique_lk(_mutex);
		//判断队列为空则用条件变量阻塞等待，并释放锁
		while (_msg_que.empty() && !_b_stop) {
			_consume.wait(unique_lk);
		}

		//判断是否为关闭状态，把所有逻辑执行完后则退出循环
		if (_b_stop) {
			while (!_msg_que.empty()) {//不为空
				std::shared_ptr<LogicNode> msg_node = _msg_que.front();
				std::cout << "recv_msg id is " << msg_node->_recvnode->_msg_id << std::endl;
				auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
				if (call_back_iter == _fun_callbacks.end()) {
					_msg_que.pop();
					continue;
				}
				call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
					std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
				_msg_que.pop();
			}

			break;
		}

		//如果没有停服，且说明队列中有数据
		std::shared_ptr<LogicNode> msg_node = _msg_que.front();
		std::cout << "recv_msg id is " << msg_node->_recvnode->_msg_id << std::endl;
		auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
		if (call_back_iter == _fun_callbacks.end()) {
			_msg_que.pop();
			std::cout << "msg id [" << msg_node->_recvnode->_msg_id << "] handler not found" << std::endl;
			continue;
		}
		call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
			std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
		_msg_que.pop();

		//std::cout << "收包处理完毕" << std::endl;
	}
}

//消息队列
void LogicSystem::PostMsgToQue(std::shared_ptr < LogicNode> msg) {
    std::unique_lock<std::mutex> unique_lk(_mutex);
    _msg_que.push(msg);

    //后续修改，设置上限，如果队列达到上限，则不让写入直接return掉
    //由0变为1则发送通知信号
    if (_msg_que.size() == 1) {
        unique_lk.unlock();
        _consume.notify_one();
    }
}

//注册一个回调函数		后续聊天相关功能都在这儿加
void LogicSystem::RegisterCallBacks() {
	//1.登录聊天账号
    _fun_callbacks[MSG_IDS::MSG_CHAT_LOGIN] = std::bind(&LogicSystem::LoginHandler, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	//2.搜索用户
	_fun_callbacks[MSG_IDS::ID_SEARCH_USER_REQ] = std::bind(&LogicSystem::SearchInfo, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	//3.添加好友申请
	_fun_callbacks[MSG_IDS::ID_ADD_FRIEND_REQ] = std::bind(&LogicSystem::AddFriendApply, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	//4.处理好友认证申请
	_fun_callbacks[MSG_IDS::ID_AUTH_FRIEND_REQ] = std::bind(&LogicSystem::AuthFriendApply, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	 
	//5.处理聊天文本消息
	_fun_callbacks[MSG_IDS::ID_TEXT_CHAT_MSG_REQ] = std::bind(&LogicSystem::DealChatTextMsg, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

//1.登录聊天账号处理函数
void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data) 
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);
	auto uid = root["uid"].asInt();
	auto token = root["token"].asString();
	std::cout << "user login uid is " << uid << " user token is "
		<< token << std::endl;

	//返回结构
	Json::Value  rtvalue;

	//在本段代码执行完后未进入Defer程序：
	/*
	分析原因：Defer捕获了rtvalue，但由于rtvalue未初始化导致Defer对于rtvalue的悬空引用
	改进方法：加入下面一行代码，设置rtvalue初始化Defer就能正常捕获rtvalue引用
	*/
	rtvalue["error"] = ErrorCodes::SUCCESS;//默认发送成功码
	Defer defer([this, &rtvalue, session]() {
		//std::cout << "Defer triggered" << std::endl; // 调试日志
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		});

	//从redis获取用户token是否正确(在内存中查询用户信息-改进为在Redis中查询用户信息)
	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
	std::string token_value = "";
	bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
	if (!success) {
		rtvalue["error"] = ErrorCodes::UidInvalid;
		return;
	}
	if (token_value != token) {
		rtvalue["error"] = ErrorCodes::TokenInvalid;
		return;
	}
	
	rtvalue["error"] = ErrorCodes::SUCCESS;//发送成功码

	//1.获取用户信息
	std::string base_key = USER_BASE_INFO + uid_str;
	auto user_info = std::make_shared<UserInfo>();
	//从Redis查询用户，如果没有则从MySQL查询，并将MySQL查询结果写入Redis
	bool b_base = GetBaseInfo(base_key, uid, user_info);
	if (!b_base) 
	{
		rtvalue["error"] = ErrorCodes::UidInvalid;
		return;
	}

	rtvalue["uid"] = uid;//uid
	rtvalue["pwd"] = user_info->pwd;//密码需要发送过去么？？
	rtvalue["name"] = user_info->name;//用户名
	rtvalue["email"] = user_info->email;//邮箱
	rtvalue["nick"] = user_info->nick;//昵称
	rtvalue["desc"] = user_info->desc;//描述
	rtvalue["sex"] = user_info->sex;//性别女为0，男为1
	rtvalue["icon"] = user_info->icon;//头像
	
	//2.从数据库获取好友申请列表
	std::vector<std::shared_ptr<ApplyInfo>> apply_list;
	auto b_apply = GetFriendApplyInfo(uid, apply_list);
	if (b_apply) {
		for (auto& apply : apply_list) {
			Json::Value obj;
			obj["name"] = apply->_name;
			obj["uid"] = apply->_uid;
			obj["icon"] = apply->_icon;
			obj["nick"] = apply->_nick;
			obj["sex"] = apply->_sex;
			obj["desc"] = apply->_desc;
			obj["status"] = apply->_status;
			rtvalue["apply_list"].append(obj);
		}
	}

	//3.从数据库获取好友列表
	std::vector<std::shared_ptr<UserInfo>> friend_list;
	bool b_friend_list = GetFriendList(uid, friend_list);
	for (auto& friend_ele : friend_list) {
		Json::Value obj;
		obj["name"] = friend_ele->name;
		obj["uid"] = friend_ele->uid;
		obj["icon"] = friend_ele->icon;
		obj["nick"] = friend_ele->nick;
		obj["sex"] = friend_ele->sex;
		obj["desc"] = friend_ele->desc;
		obj["back"] = friend_ele->back;
		rtvalue["friend_list"].append(obj);
	}

	//登录数量统计
	auto server_name = ConfigMgr::Inst().GetValue("SelfServer", "Name");
	//将登录数量增加
	auto rd_res = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server_name);
	int count = 0;
	if (!rd_res.empty()) {//不为空，获取Redis中连接数量
		count = std::stoi(rd_res);
	}
	//为空，自增
	count++;
	auto count_str = std::to_string(count);
	RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, count_str);
	//session绑定用户uid
	session->SetUserId(uid);
	//为用户设置登录ip server的名字
	std::string ipkey = USERIPPREFIX + uid_str;

	std::cout << "ipkey:" << ipkey << " server_name:" << server_name << std::endl;
	RedisMgr::GetInstance()->Set(ipkey, server_name);
	//uid和session绑定管理,方便以后踢人操作
	UserMgr::GetInstance()->SetUserSession(uid, session);

	return;
}

//2.搜索好友处理函数
void LogicSystem::SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);

	std::string query_str = root["querystr"].asString();//这里使用uid字段来传递uid或者name的
	std::cout << "user SearchInfo str is  " << query_str <<std::endl;


	//返回JSON结构
	Json::Value rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;//默认发送成功码
	Defer defer([this, &rtvalue, session]() {
		//std::cout << "Defer triggered" << std::endl; // 调试日志
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_IDS::ID_SEARCH_USER_RSP);//消息体+ID
		});

	bool b_digit = isPureDigit(query_str);
	if (b_digit)//是纯数字
	{
		rtvalue["error"] = ErrorCodes::SUCCESS;//没有错误
		GetUserByUid(query_str, rtvalue);//rtvalue为回调参数，存储查询结果
	}
	else//不是纯数字
	{
		rtvalue["error"] = ErrorCodes::SUCCESS;//没有错误
		GetUserByName(query_str, rtvalue);//rtvalue为回调参数，存储查询结果
	}
}

//3.添加好友申请处理函数
void LogicSystem::AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);

	auto uid = root["uid"].asInt();
	auto applyname = root["applyname"].asString();
	auto backname = root["backname"].asString();
	auto touid = root["touid"].asInt();

	//返回JSON结构
	Json::Value rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;//默认发送成功码
	Defer defer([this, &rtvalue, session]() {
		std::cout << "Defer triggered" << std::endl; // 调试日志
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_IDS::ID_ADD_FRIEND_RSP);//消息体+ID
		});

	//先更新数据库，将好友申请关系写入数据库
	MysqlMgr::GetInstance()->AddFriendApply(uid, touid);

	//查找Redis 查询touid对应的server ip
	std::string to_str = std::to_string(touid);
	std::string to_ip_key = USERIPPREFIX + to_str;
	std::string to_ip_value = "";//查询结果放入此
	bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
	if (!b_ip)//没查到，说明对方都没有登录到任意服务器
	{
		return;
	}
	//查到，需要判断对方是否跟本人在同一个服务器
	//读取自己的服务器配置
	ConfigMgr& cfg = ConfigMgr::Inst();
	std::string self_name = cfg["SelfServer"]["Name"];

	//查询自己的信息，发送给对方服务器需要用到此信息
	std::string base_key = USER_BASE_INFO + std::to_string(uid);
	auto apply_info = std::make_shared<UserInfo>();
	bool b_info = GetBaseInfo(base_key, uid, apply_info);//根据base_key和uid获取到自己的用户信息

	//在本服务器，直接转发
	if (to_ip_value == self_name)
	{
		std::shared_ptr<CSession> session = UserMgr::GetInstance()->GetSession(touid);
		if (session) //找到对应的session
		{
			//对方在线，直接发送自己的信息通知对方
			Json::Value rtvalue;
			rtvalue["error"] = ErrorCodes::SUCCESS;
			rtvalue["applyuid"] = uid;
			rtvalue["name"] = applyname;
			rtvalue["desc"] = apply_info->desc;
			if (b_info) {
				rtvalue["icon"] = apply_info->icon;
				rtvalue["sex"] = apply_info->sex;
				rtvalue["nick"] = apply_info->nick;
			}
			std::string return_str = rtvalue.toStyledString();
			session->Send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
		}
		return;//没找到session，不在内存中（对方不在线），不作处理？？？？
	}
	//在其他服务器，需要通过GRPC转发给对方所在服务器
	AddFriendReq add_req;
	add_req.set_applyuid(uid);//自己uid
	add_req.set_touid(touid);//对方uid
	add_req.set_name(applyname);
	add_req.set_desc(apply_info->desc);
	if (b_info)
	{
		add_req.set_icon(apply_info->icon);//头像
		add_req.set_sex(apply_info->sex); //性别
		add_req.set_nick(apply_info->nick);//昵称
	}
	//通知服务器
	ChatGrpcClient::GetInstance()->NotifyAddFriend(to_ip_value/*对端server*/, add_req/*请求消息内容*/);//添加好友不用回包
}

//4.处理好友认证申请处理函数
void LogicSystem::AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);

	auto uid = root["fromuid"].asInt();
	auto touid = root["touid"].asInt();
	auto back_name = root["back"].asString();
	std::cout << "AUTH from " << uid << " auth friend to " << touid << std::endl;

	Json::Value  rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;
	auto user_info = std::make_shared<UserInfo>();

	std::string base_key = USER_BASE_INFO + std::to_string(touid);
	bool b_info = GetBaseInfo(base_key, touid, user_info);
	if (b_info) {
		rtvalue["name"] = user_info->name;
		rtvalue["nick"] = user_info->nick;
		rtvalue["icon"] = user_info->icon;
		rtvalue["sex"] = user_info->sex;
		rtvalue["uid"] = touid;
	}
	else {
		rtvalue["error"] = ErrorCodes::UidInvalid;
	}

	Defer defer([this, &rtvalue, session]() {
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, ID_AUTH_FRIEND_RSP);
		});

	//先更新数据库
	MysqlMgr::GetInstance()->AuthFriendApply(uid, touid);	//更新friend_apply表的状态为已认证（Status 0表示未认证，1表示已认证）
	//再更新数据库添加好友
	MysqlMgr::GetInstance()->AddFriend(uid, touid, back_name);	//friend表添加好友关系

	//查询redis 查找touid对应的server ip（添加好友对方所在的聊天服务器）
	auto to_str = std::to_string(touid);
	auto to_ip_key = USERIPPREFIX + to_str;
	std::string to_ip_value = "";
	bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
	if (!b_ip) {
		return;
	}

	auto& cfg = ConfigMgr::Inst();
	auto self_name = cfg["SelfServer"]["Name"];
	//如果对方在本聊天服务器，直接通知对方有认证通过消息
	if (to_ip_value == self_name) {
		auto session = UserMgr::GetInstance()->GetSession(touid);
		if (session) {
			//在内存中则直接发送通知对方
			Json::Value rtvalue;
			rtvalue["error"] = ErrorCodes::SUCCESS;
			rtvalue["fromuid"] = uid;
			rtvalue["touid"] = touid;
			std::string base_key = USER_BASE_INFO + std::to_string(uid);//获取自己的用户信息key
			auto user_info = std::make_shared<UserInfo>();
			bool b_info = GetBaseInfo(base_key, uid, user_info);
			if (b_info) {
				rtvalue["name"] = user_info->name;
				rtvalue["nick"] = user_info->nick;
				rtvalue["icon"] = user_info->icon;
				rtvalue["sex"] = user_info->sex;
			}
			else {
				rtvalue["error"] = ErrorCodes::UidInvalid;
			}

			std::string return_str = rtvalue.toStyledString();
			session->Send(return_str, ID_NOTIFY_AUTH_FRIEND_REQ);
		}
		return;
	}

	//对方在其他聊天服务器上，则通过GRPC服务通知对方服务器
	AuthFriendReq auth_req;
	auth_req.set_fromuid(uid);//本人uid
	auth_req.set_touid(touid);//对方uid
	ChatGrpcClient::GetInstance()->NotifyAuthFriend(to_ip_value, auth_req);
}

//5.处理文本聊天消息
void LogicSystem::DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data)
{
	//接收JSON结构
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);

	auto uid = root["fromuid"].asInt();//发送者uid
	auto touid = root["touid"].asInt();//接收者uid

	const Json::Value arrays = root["text_array"];//文本数组

	//返回JSON结构
	Json::Value rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;//默认发送成功码
	rtvalue["text_array"] = arrays;
	rtvalue["fromuid"] = uid;
	rtvalue["touid"] = touid;

	Defer defer([this, &rtvalue, session]() {
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_IDS::ID_TEXT_CHAT_MSG_RSP);//消息体+ID
		std::cout << "ID_TEXT_CHAT_MSG_RSP success" << std::endl;
		});

	//查询Redis 查找touid对应的server ip
	auto to_str = std::to_string(touid);
	auto to_ip_key = USERIPPREFIX + to_str;
	std::string to_ip_value = "";
	bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
	if (!b_ip)
	{
		return;
	}

	//读取自己的服务器配置
	auto& cfg = ConfigMgr::Inst();
	auto self_name = cfg["SelfServer"]["Name"];
	//如果对方在本聊天服务器，直接推送给对方聊天信息
	if (to_ip_value == self_name)
	{
		auto session = UserMgr::GetInstance()->GetSession(touid);
		if (session)
		{
			//在内存中则直接发送通知对方
			std::string return_str = rtvalue.toStyledString();
			session->Send(return_str, MSG_IDS::ID_NOTIFY_TEXT_CHAT_MSG_REQ);
			std::cout << "ID_NOTIFY_TEXT_CHAT_MSG_REQ success" << std::endl;
		}
		return;//没找到session，不在内存中（对方不在线），不作处理
	}

	//对方在其他聊天服务器上，则通过GRPC服务通知对方服务器
	TextChatMsgReq text_msg_req;
	text_msg_req.set_fromuid(uid);//发送者uid	//添加第一字段
	text_msg_req.set_touid(touid);//接收者uid	//添加第二字段
	for (const auto& txt_obj : arrays) {
		auto content = txt_obj["content"].asString();
		auto msgid = txt_obj["msgid"].asString();
		std::cout << "content is " << content << ", msgid is " << msgid << std::endl;
		TextChatData* text_msg = text_msg_req.add_textmsgs();//添加第三字段
		text_msg->set_msgid(msgid);//设置消息ID
		text_msg->set_msgcontent(content);//设置文本内容
	}
	//通知服务器
	ChatGrpcClient::GetInstance()->NotifyTextChatMsg(to_ip_value, text_msg_req, rtvalue);


	//todo...
}

//判断字符串是否为纯数字	用于区分名字和uid
bool LogicSystem::isPureDigit(const std::string& str)
{
	for (char c : str)
	{
		if (!std::isdigit(c))
		{
			return false;//不是纯数字
		}
	}
	return true;//是纯数字
}

//通过uid获取用户信息
void LogicSystem::GetUserByUid(std::string uid_str, Json::Value& rtvalue)
{
	//1.从Redis中查找用户信息
	std::string base_key = USER_BASE_INFO + uid_str;//设置查询的key
	std::string info_str = "";
	//通过base_key查询，查询结果放入info_str
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)//查到
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		int uid = root["uid"].asInt();
		std::string name = root["name"].asString();
		std::string pwd = root["pwd"].asString();
		std::string email = root["email"].asString();
		std::string nick = root["nick"].asString();
		std::string desc = root["desc"].asString();
		int sex = root["sex"].asInt();
		std::string icon = root["icon"].asString();
		std::cout << "user  uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email << " icon is " << icon << std::endl;

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		rtvalue["icon"] = icon;
		return;
	}

	//没有查到,则在MySQL中查询，并将MySQL中查询的结果写入Redis
	//2.Redis中没有则查询MySQL
	auto uid = std::stoi(uid_str);
	std::shared_ptr<UserInfo> user_info = nullptr;//创建一个UserInfo结构体存储MySQL查询结果
	user_info = MysqlMgr::GetInstance()->GetUser(uid);
	if (user_info == nullptr) 
	{
		rtvalue["error"] = ErrorCodes::UidInvalid;
		return;
	}

	//将数据库内容写入redis缓存
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	redis_root["icon"] = user_info->icon;
	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

	//返回数据给客户端
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
	rtvalue["icon"] = user_info->icon;
}

//通过名字获取用户信息
void LogicSystem::GetUserByName(std::string name, Json::Value& rtvalue)
{
	//1.从Redis中查找用户信息
	std::string base_key = NAME_INFO + name;//设置查询的key
	std::string info_str = "";
	//通过base_key查询，查询结果放入info_str
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base) //查到
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		int uid = root["uid"].asInt();
		std::string name = root["name"].asString();
		std::string pwd = root["pwd"].asString();
		std::string email = root["email"].asString();
		std::string nick = root["nick"].asString();
		std::string desc = root["desc"].asString();
		int sex = root["sex"].asInt();
		std::cout << "Redis Result: user uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email << std::endl;

		//返回数据
		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		return;
	}

	//没有查到,则在MySQL中查询，并将MySQL中查询的结果写入Redis
	//2.Redis中没有则查询MySQL
	std::shared_ptr<UserInfo> user_info = nullptr;//创建一个UserInfo结构体存储MySQL查询结果
	user_info = MysqlMgr::GetInstance()->GetUser(name);
	if (user_info == nullptr) 
	{
		rtvalue["error"] = ErrorCodes::UidInvalid;
		return;
	}

	//将数据库内容写入redis缓存
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

	std::cout << "MySQL Result: user uid is  " << user_info->uid << " name  is "
		<< user_info->name << " pwd is " << user_info->pwd << " email is " << user_info->email << std::endl;

	//返回数据
	rtvalue["uid"] = user_info->uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
}

//获取用户基本信息
bool LogicSystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
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
		std::cout << "user uid is  " << userinfo->uid << " name  is "
			<< userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << std::endl;
	}
	else {
		//Redis中没有则查询mysql
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

//获取好友申请信息
bool LogicSystem::GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list) 
{
	//从mysql获取好友申请列表
	return MysqlMgr::GetInstance()->GetApplyList(to_uid, list, 0, 10);
}

bool LogicSystem::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list) {
	//从mysql获取好友列表
	return MysqlMgr::GetInstance()->GetFriendList(self_id, user_list);
}
