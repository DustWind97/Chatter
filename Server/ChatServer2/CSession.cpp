﻿#include "CSession.h"
#include "CServer.h"
#include <iostream>
//#include <sstream>
#include "LogicSystem.h"
#include "RedisMgr.h"

CSession::CSession(boost::asio::io_context& io_context, CServer* server):
    _socket(io_context), _server(server), _b_close(false), _b_head_parse(false), _user_uid(0)
{
	//通过boost库的雪花算法生成一个随机的uuid
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    _session_id = boost::uuids::to_string(a_uuid);//将这一uuid转换成字符串作为会话id
    _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);//初始化消息节点
    _last_heartbeat = std::time(nullptr);
}

CSession::~CSession()
{
	std::cout << "CSession is Destructed" << std::endl;
}

tcp::socket& CSession::GetSocket()
{
    return _socket;
}

std::string& CSession::GetSessionId()
{
	return _session_id;
}

void CSession::SetUserId(int uid)
{
    _user_uid = uid;
}

int CSession::GetUserId()
{
    return _user_uid;
}

void CSession::Start()
{
	AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::Send(std::string msg, short msgid) {
    std::lock_guard<std::mutex> lock(_send_lock);
    size_t send_que_size = _send_que.size();//int改为了size_t
    if (send_que_size > MAX_SENDQUE) {
        std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    _send_que.push(std::make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
    if (send_que_size > 0) {
        return;
    }
    auto& msgnode = _send_que.front();
    boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
        std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Send(char* msg, short max_length, short msgid) {
    std::lock_guard<std::mutex> lock(_send_lock);
    size_t send_que_size = _send_que.size();//int改为了size_t
    if (send_que_size > MAX_SENDQUE) {
        std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    _send_que.push(std::make_shared<SendNode>(msg, max_length, msgid));
    if (send_que_size > 0) {
        return;
    }
    auto& msgnode = _send_que.front();
    boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
        std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Close()
{
}

std::shared_ptr<CSession> CSession::SharedSelf()
{
    return shared_from_this();
}

//异步读取消息头
void CSession::AsyncReadHead(int total_len)
{
    auto self = shared_from_this();
    asyncReadFull(HEAD_TOTAL_LEN, [self, this](const boost::system::error_code& ec, std::size_t bytes_transfered) {
		//只有读取全或者四字节才会调用这个回调函数
        try {
            if (ec) {
                std::cout << "handle read failed, error is " << ec.what() << std::endl;
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            //此处代码可以优化掉，还是留着吧
            if (bytes_transfered < HEAD_TOTAL_LEN) {
                std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
                    << HEAD_TOTAL_LEN << "]" << std::endl;
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            _recv_head_node->Clear();
            memcpy(_recv_head_node->_data, _data, bytes_transfered);

            //获取头部MSGID数据
            short msg_id = 0;
            memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
            //网络字节序转化为本地字节序    如果msg_id是小端不作处理，如果是大端则将大端类型转换成本地的host短整型（小端） 
            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
            std::cout << "msg_id is " << msg_id << std::endl;
            //id非法
            if (msg_id > MAX_LENGTH) {
                std::cout << "invalid msg_id is " << msg_id << std::endl;
                _server->ClearSession(_session_id);
                return;
            }
            short msg_len = 0;
            memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
            //网络字节序转化为本地字节序     如果msg_len是小端不作处理，如果是大端则将大端类型转换成本地的host短整型（小端） 
            msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
            std::cout << "msg_len is " << msg_len << std::endl;

            //id非法
            if (msg_len > MAX_LENGTH) {
                std::cout << "invalid data length is " << msg_len << std::endl;
                _server->ClearSession(_session_id);//_uuid改成了_session_id
                return;
            }

            _recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);
            AsyncReadBody(msg_len);
        }
        catch (std::exception& e) 
        {
            std::cout << "Exception code is " << e.what() << std::endl;
        }
        });
}

void CSession::NotifyOffline(int uid)
{
    Json::Value  rtvalue;
    rtvalue["error"] = ErrorCodes::SUCCESS;
    rtvalue["uid"] = uid;


    std::string return_str = rtvalue.toStyledString();

    Send(return_str, ID_NOTIFY_OFF_LINE_REQ);
    return;
}

bool CSession::IsHeartbeatExpired(std::time_t& now)
{
    double diff_sec = std::difftime(now, _last_heartbeat);
    if (diff_sec > 20) {
        std::cout << "heartbeat expired, session id is  " << _session_id << std::endl;
        return true;
    }

    return false;
}

void CSession::UpdateHeartbeat()
{
    time_t now = std::time(nullptr);
    _last_heartbeat = now;
}

void CSession::DealExceptionSession()
{
    //auto self = shared_from_this();
    ////加锁清除session
    //auto uid_str = std::to_string(_user_uid);
    //auto lock_key = LOCK_PREFIX + uid_str;
    //auto identifier = RedisMgr::GetInstance()->acquireLock(lock_key, LOCK_TIME_OUT, ACQUIRE_TIME_OUT);
    //Defer defer([identifier, lock_key, self, this]() {
    //    _server->ClearSession(_session_id);
    //    RedisMgr::GetInstance()->releaseLock(lock_key, identifier);
    //    });

    //if (identifier.empty()) {
    //    return;
    //}
    //std::string redis_session_id = "";
    //auto bsuccess = RedisMgr::GetInstance()->Get(USER_SESSION_PREFIX + uid_str, redis_session_id);
    //if (!bsuccess) {
    //    return;
    //}

    //if (redis_session_id != _session_id) {
    //    //说明有客户在其他服务器异地登录了
    //    return;
    //}

    //RedisMgr::GetInstance()->Del(USER_SESSION_PREFIX + uid_str);
    ////清除用户登录信息
    //RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);
}

//异步读取消息体
void CSession::AsyncReadBody(int total_len)
{
    auto self = shared_from_this();
    asyncReadFull(total_len, [self, this, total_len](const boost::system::error_code& ec, std::size_t bytes_transfered) {
        try {
            if (ec) {
                std::cout << "handle read failed, error is " << ec.what() << std::endl;
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            if (bytes_transfered < total_len) {
                std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
                    << total_len << "]" << std::endl;
                Close();
                _server->ClearSession(_session_id);
                return;
            }

            memcpy(_recv_msg_node->_data, _data, bytes_transfered);
            _recv_msg_node->_cur_len += bytes_transfered;
            _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
            std::cout << "receive data is " << _recv_msg_node->_data << std::endl;
            //此处将消息投递到逻辑队列中
            LogicSystem::GetInstance()->PostMsgToQue(std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
            //继续监听头部接受事件
            AsyncReadHead(HEAD_TOTAL_LEN);//进行下一次接收
        }
        catch (std::exception& e) {
            std::cout << "Exception code is " << e.what() << std::endl;
        }
        });
}

//异步读取所有    必须很完整地读取四字节才会触发回调
void CSession::asyncReadFull(std::size_t maxLength,
    std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	::memset(_data, 0, MAX_LENGTH);
	asyncReadLen(0, maxLength, handler);//从0开始读取，读取maxLength长度并交个handler处理
}

//异步读取消息长度
void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len,
    std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	auto self = shared_from_this();
    //data域首地址+已经读的长度
    _socket.async_read_some(boost::asio::buffer(_data + read_len, total_len-read_len),
        [read_len, total_len, handler, self](const boost::system::error_code& ec, std::size_t bytesTransfered) {
            if (ec)
            {
                //出现错误，调用回调函数
                handler(ec,read_len+bytesTransfered);
                return;
            }

            if (read_len+bytesTransfered >= total_len)
            {
				//长度超过了总长度，调用回调函数
                handler(ec, read_len + bytesTransfered);
                return;
            }

			//没有错误且长度不足则继续读取
            self->asyncReadLen(read_len + bytesTransfered,total_len,handler);
        });
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self)
{
    //增加异常处理
    try {
        if (!error) {
            std::lock_guard<std::mutex> lock(_send_lock);
            //cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
            _send_que.pop();
            if (!_send_que.empty()) {
                auto& msgnode = _send_que.front();
                boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
                    std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
            }
        }
        else {
            std::cout << "handle write failed, error is " << error.what() << std::endl;
            Close();
            _server->ClearSession(_session_id);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception code : " << e.what() << std::endl;
    }
}


//逻辑节点 构造函数
LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode):
	_session(session), _recvnode(recvnode)
{
}
