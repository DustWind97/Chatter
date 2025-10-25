#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h> 
#include "message.grpc.pb.h"
#include "message.pb.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

class ChatConPool {
public:
	ChatConPool(size_t poolSize, std::string host, std::string port);//池子大小，对端IP，对端Port
	~ChatConPool();
	std::unique_ptr<ChatService::Stub> getConnection();//获取连接
	void returnConnection(std::unique_ptr<ChatService::Stub> context);//返回连接
	void Close();//关闭

private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<ChatService::Stub>> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

class ChatGrpcClient :public Singleton<ChatGrpcClient>
{
	friend class Singleton<ChatGrpcClient>;
public:
	~ChatGrpcClient();

	AddFriendRsp NotifyAddFriend(const AddFriendReq& req);//通知添加好友

private:
	ChatGrpcClient();
	//该服务器的连接池情况参见ChatConPool：连接数，对端IP和端口
	std::unordered_map<std::string, std::unique_ptr<ChatConPool>> _pools;
};

/** 此组件未被使用 */

