#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <mutex>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class  ChatServer 
{
public:
	//默认构造函数
	ChatServer():host(""), port(""), name(""), con_count(0/*INT_MAX*/) {}
	//拷贝构造
	ChatServer(const ChatServer& cs) :host(cs.host), port(cs.port), name(cs.name), con_count(cs.con_count) {}
	//拷贝赋值操作符重载
	ChatServer& operator=(const ChatServer& cs) {
		if (&cs == this) {
			return *this;
		}

		host = cs.host;
		name = cs.name;
		port = cs.port;
		con_count = cs.con_count;
		return *this;
	}
	std::string host; // 聊天服务器IP地址
	std::string port; // 聊天服务器端口
	std::string name; // 聊天服务器名
	int con_count;	  // 连接计数
};

//状态服务接口 final关键字防止继承和重写操作
class StatusServiceImpl final : public StatusService::Service
{
public:
	StatusServiceImpl();
	//StatusGrpc Sever端服务
	Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
						 GetChatServerRsp* reply) override;
	//StatusGrpc Sever端服务，这个函数现如今没用上（之前单服务器时状态服务器与聊天服务器grpc通信用，现已改为分布式聊天服务器）
	Status Login(ServerContext* context, const LoginReq* request, LoginRsp* reply) override;

private:
	void insertToken(int uid, std::string token);//往Redis中添加token
	ChatServer getChatServer();

private:
	std::unordered_map<std::string, ChatServer> _servers;//记录聊天服务器信息
	std::mutex _server_mtx;//服务器资源访问锁
};

