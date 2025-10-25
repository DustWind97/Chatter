#pragma once
#include <string>
#include "const.h"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class LogicSystem;
//消息节点基类
class MsgNode
{
public:
	MsgNode(short max_len);
	~MsgNode();
	void Clear();
	short _cur_len;//当前长度
	short _total_len;//总长度
	char* _data;//数据域
};

//接收节点类
class RecvNode :public MsgNode {
	friend class LogicSystem;
public:
	RecvNode(short max_len, short msg_id);
private:
	short _msg_id;
};

//发送节点类
class SendNode :public MsgNode {
	friend class LogicSystem;
public:
	SendNode(const char* msg, short max_len, short msg_id);
private:
	short _msg_id;
};

