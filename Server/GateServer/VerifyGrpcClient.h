#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;//状态
using grpc::ClientContext;//上下文

using message::VerifyService;//验证服务
using message::GetVerifyReq;//请求Request
using message::GetVerifyRsp;//回复Response


class RPConPool{
public:
	RPConPool(size_t poolsize, std::string host, std::string port);
	~RPConPool();
	void Close();

	std::unique_ptr<VerifyService::Stub> getConnection();
	void returnConnection(std::unique_ptr<VerifyService::Stub> context);

private:
	std::atomic<bool> b_stop_; //原子化安全一些
	size_t poolSize_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<VerifyService::Stub>> connections_;//用队列管理连接池
	std::condition_variable cond_;//状态
	std::mutex mutex_;//加锁
};


class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	//外层给GateServer用
	GetVerifyRsp GetVerifyCode(std::string email);

private:
	VerifyGrpcClient();
	std::unique_ptr<RPConPool> pool_;

};

