#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "const.h"
#include "RedisMgr.h"
#include <climits>

//构造函数
StatusServiceImpl::StatusServiceImpl()
{
	//解析配置文件，取出 聊天服务器相关字段
	auto& cfg = ConfigMgr::Inst();
	auto server_list = cfg["chatservers"]["Name"];

	std::vector<std::string> words;
	std::stringstream ss(server_list);
	std::string word;

	//由于对端服务器可能有多个，chatservers字符串类型的服务器组，用逗号分隔的(注意是英文半角逗号)
	//将这一组服务器解析出来
	while (std::getline(ss, word, ',')) {
		words.push_back(word);
	}

	// 调试输出加载的服务器
	std::cout << "Loading chat servers from config:" << std::endl;
	for (auto& word : words) {
		std::cout << "- Server config key: " << word << std::endl;
	}

	//遍历服务器组
	for (auto& word : words) {
		if (cfg[word]["Name"].empty()) 
		{
			std::cerr << "Warning: Missing config for server: " << word << std::endl;
			continue;
		}

		ChatServer server;
		server.port = cfg[word]["Port"];
		server.host = cfg[word]["Host"];
		server.name = cfg[word]["Name"];

		// 调试输出服务器信息
		std::cout << "Loaded server: " << server.name
			<< " (" << server.host << ":" << server.port << ")" << std::endl;

		_servers[server.name] = server;
	}

	// 检查是否成功加载服务器
	if (_servers.empty()) {
		std::cerr << "CRITICAL ERROR: No chat servers loaded!" << std::endl;
	}
	else {
		std::cout << "Successfully loaded " << _servers.size()
			<< " chat server(s)" << std::endl;
	}
}

//生成唯一的uuid字符串作为token
std::string generate_unique_string() {
	// 创建UUID对象
	boost::uuids::uuid uuid = boost::uuids::random_generator()();//通过boost库的雪花算法生成UUID
	// 将UUID转换为字符串
	std::string unique_string = to_string(uuid);
	return unique_string;
}

//获取聊天服务器
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
	std::cout << "\nReceived GetChatServer request from UID: " << request->uid() << std::endl;

	std::string prefix("dustwind status server has received :  ");
	const ChatServer& server = getChatServer();

	std::cout << "Selected server: " << server.name
		<< " (" << server.host << ":" << server.port
		<< ") with connections: " << server.con_count << std::endl;

	reply->set_host(server.host);	//这些方法都是通过proto文件生成并在protobuf里定义的
	reply->set_port(server.port);
	reply->set_error(ErrorCodes::SUCCESS);
	reply->set_token(generate_unique_string());
	insertToken(request->uid(), reply->token());//回复GateServer并将token存入Redis缓存中
	return Status::OK;
}

//获取聊天服务器的具体实现
ChatServer StatusServiceImpl::getChatServer() 
{
	std::lock_guard<std::mutex> guard(_server_mtx);//上锁，保证线程安全

	// 调试输出当前服务器列表
	std::cout << "\nAvailable servers (" << _servers.size() << "):" << std::endl;
	for (const auto& server : _servers) {
		std::cout << "- " << server.second.name << std::endl;
	}

	ChatServer minServer = _servers.begin()->second;//从server池里获取一个聊天服务器，初始设定为迭代器开始的那一个
	auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, minServer.name);  //???????此处会触发Redis五次认证
	if (count_str.empty()) {
		//不存在则默认设置为最大
		minServer.con_count = INT_MAX;
	}
	else {
		minServer.con_count = std::stoi(count_str);//将字符串转为整形
	}

	// 使用范围基于for循环		找到负载最小连接数的服务器
	for (auto& server : _servers) {
		if (server.second.name == minServer.name) {//跳过默认赋值那一个服务器
			continue;
		}

		auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server.second.name);
		// 调试输出Redis查询结果
		std::cout << "Checking server: " << server.second.name
			<< " - Redis count: " << (count_str.empty() ? "N/A" : count_str) << std::endl;

		if (count_str.empty()) {
			server.second.con_count = INT_MAX;
			std::cout << " (using INT_MAX)" << std::endl;
		}
		else 
		{
			server.second.con_count = std::stoi(count_str);//将字符串转为整形
		}
		if (server.second.con_count < minServer.con_count) {
			minServer = server.second;
		}
	}
	return minServer;

	/************************************************************************************************/
	////修改处理逻辑
	//std::lock_guard<std::mutex> guard(_server_mtx);
	////auto minServer = _servers.begin()->second;//调试用
	//// 调试输出当前服务器列表
	//std::cout << "\nAvailable servers (" << _servers.size() << "):" << std::endl;
	//for (const auto& server : _servers) {
	//	std::cout << "- " << server.second.name << std::endl;
	//}
	//// 初始化为无效值
	//ChatServer minServer;
	//minServer.con_count = INT_MAX;
	//bool foundValid = false;
	//// 遍历所有服务器，实时获取连接数
	//for (auto& server : _servers) {
	//	auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server.second.name);
	//	// 调试输出Redis查询结果
	//	std::cout << "Checking server: " << server.second.name
	//		<< " - Redis count: " << (count_str.empty() ? "N/A" : count_str);
	//	if (count_str.empty()) {
	//		server.second.con_count = INT_MAX;
	//		std::cout << " (using INT_MAX)" << std::endl;
	//	}
	//	else {
	//		try {
	//			server.second.con_count = std::stoi(count_str);
	//			std::cout << " (using " << server.second.con_count << ")" << std::endl;
	//		}
	//		catch (...) {
	//			server.second.con_count = INT_MAX;
	//			std::cerr << " - ERROR converting Redis value: " << count_str << std::endl;
	//		}
	//	}
	//	// 找到连接数最小的服务器
	//	if (server.second.con_count < minServer.con_count) {
	//		minServer = server.second;
	//		foundValid = true;
	//	}
	//}
	//// 修复3: 添加fallback机制
	//if (!foundValid && !_servers.empty()) {
	//	minServer = _servers.begin()->second;
	//	std::cerr << "WARNING: Using fallback server - " << minServer.name
	//		<< " (no valid Redis data)" << std::endl;
	//}
	/************************************************************************************************/
}

//登录接口
Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
	std::int32_t uid = request->uid();
	std::string token = request->token();

	//查询Redis
	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
	std::string token_value = "";
	bool success = RedisMgr::GetInstance()->Get(token_key, token_value);

	//查询不成功
	if (success) {
		reply->set_error(ErrorCodes::UidInvalid);
		return Status::OK;
	}

	//token不匹配
	if (token_value != token) {
		reply->set_error(ErrorCodes::TokenInvalid);
		return Status::OK;
	}

	//设置回复
	reply->set_error(ErrorCodes::SUCCESS);
	reply->set_uid(uid);
	reply->set_token(token);
	return Status::OK;
}

void StatusServiceImpl::insertToken(int uid, std::string token)
{
	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
	RedisMgr::GetInstance()->Set(token_key, token);
}

