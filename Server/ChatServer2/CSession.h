#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
//#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <memory>
#include "const.h"
#include "MsgNode.h"
//#include "CServer.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class CServer;
class LogicSystem;
class CSession : public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();
	tcp::socket& GetSocket();//获取socket
	std::string& GetSessionId();//获取会话id
	void SetUserId(int uid);//设置用户id
	int  GetUserId();//获取用户id

	void Start();
	//两种类型的Send方法一种是告知消息长度，一种是直接发送字符串 异步发送应当用消息队列保证有序性
	void Send(char* msg, short max_length, short msgid);
	void Send(std::string msg, short msgid);
	void Close();//关闭
	std::shared_ptr<CSession> SharedSelf();
	void AsyncReadBody(int total_len);
	void AsyncReadHead(int total_len);

	//添加心跳机制
	void NotifyOffline(int uid);
	//判断心跳是否过期
	bool IsHeartbeatExpired(std::time_t& now);
	//更新心跳
	void UpdateHeartbeat();
	//处理异常连接
	void DealExceptionSession();

private:
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler);
	void asyncReadLen(std::size_t  read_len, std::size_t total_len,
					  std::function<void(const boost::system::error_code&, std::size_t)> handler);
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);

private:
	tcp::socket _socket;								//套接字
	std::string _session_id;							//会话id
	char _data[MAX_LENGTH];								//数据缓存区
	CServer* _server;									//服务器
	bool _b_close;										//是否关闭
	std::queue<std::shared_ptr<SendNode> > _send_que;	//发送队列
	std::mutex _send_lock;								//send发送锁
	//收到的消息结构
	std::shared_ptr<RecvNode> _recv_msg_node;			//接收消息结构
	bool _b_head_parse;									//判断头部是否解析完（同Qt客户端的 _b_padding 是一个作用）
	//收到的头部结构
	std::shared_ptr<MsgNode> _recv_head_node;			//接收头部结构
	int _user_uid;										//用户uid
	//记录上次接受数据的时间
	std::atomic<time_t> _last_heartbeat;				//上次心跳
	std::mutex _session_mtx;							//session会话锁
};

class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode);
private:
	std::shared_ptr<CSession> _session;
	std::shared_ptr<RecvNode> _recvnode;
};


