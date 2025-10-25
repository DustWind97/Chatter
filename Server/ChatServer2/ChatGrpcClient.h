#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h> 
#include "message.grpc.pb.h"
#include "message.pb.h"
//#include <queue>
#include "data.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

//RPC命名空间
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

//添加好友
using message::AddFriendReq;
using message::AddFriendRsp;

//认证好友
using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;//获取聊天服务 回复
using message::LoginReq;//登录请求
using message::LoginRsp;//登陆回复
using message::ChatService;//聊天服务

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

//聊天连接池，保证线程安全
class ChatConPool {
public:
	ChatConPool(size_t poolSize, std::string host, std::string port);//池子大小，对端IP，对端Port
	~ChatConPool();
	std::unique_ptr<ChatService::Stub> getConnection();//获取连接
	void returnConnection(std::unique_ptr<ChatService::Stub> context);//返回连接
	void Close();//关闭

private:
	std::atomic<bool> b_stop_;//是否停止连接池
	size_t poolSize_;//连接池大小
	std::string host_;//对端IP地址
	std::string port_;//对端端口
	std::queue<std::unique_ptr<ChatService::Stub>> connections_;//连接队列
	//互斥锁加条件变量配合连接池访问（生产者消费者模型）
	std::mutex mutex_;//互斥锁
	std::condition_variable cond_;//条件变量
};

//聊天GRPC客户端
class ChatGrpcClient :public Singleton<ChatGrpcClient>
{
	friend class Singleton<ChatGrpcClient>;
public:
	~ChatGrpcClient();

	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	//通知添加好友
	AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);
	//通知认证好友
	AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);
	//通知文本聊天消息
	TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);

private:
	ChatGrpcClient();
	//该服务器的连接池情况参见ChatConPool：连接数，对端IP和端口
	std::unordered_map<std::string /*服务器名字*/, std::unique_ptr<ChatConPool>/*该服务器的连接池情况*/> _pools;
};