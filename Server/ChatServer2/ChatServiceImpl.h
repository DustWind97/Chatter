#pragma once
#include <grpcpp/grpcpp.h> 
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <mutex>
#include "data.h"

//RPC命名空间
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

//添加好友
using message::AddFriendReq;
using message::AddFriendRsp;

//认证好友
using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::ChatService;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatServiceImpl final/*基类到这里终结类*/ : public ChatService::Service
{
public:
	ChatServiceImpl();
	~ChatServiceImpl();
	//1.获取基本信息
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	//2.通知添加好友
	Status NotifyAddFriend(::grpc::ServerContext* context, const AddFriendReq* request, AddFriendRsp* reply) override;
	//3.通知认证好友
	Status NotifyAuthFriend(::grpc::ServerContext* context,const AuthFriendReq* request, AuthFriendRsp* response) override;
	//4.通知文本聊天消息
	Status NotifyTextChatMsg(::grpc::ServerContext* context, const TextChatMsgReq* request, TextChatMsgRsp* response) override;

private:
};

