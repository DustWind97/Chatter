﻿#include "MsgNode.h"

/********************************************************************************************************/
//消息节点基类构造函数
MsgNode::MsgNode(short max_len) :_total_len(max_len), _cur_len(0) 
{
	//初始化数据域
	_data = new char[_total_len + 1]();
	_data[_total_len] = '\0';
}

MsgNode::~MsgNode()
{
	std::cout << "destruct MsgNode" << std::endl;
	delete[] _data;
}

void MsgNode::Clear()
{
	::memset(_data, 0, _total_len);
	_cur_len = 0;
}

/********************************************************************************************************/
//发送节点类构造函数
SendNode::SendNode(const char* msg, short max_len, short msg_id) :MsgNode(max_len + HEAD_TOTAL_LEN)
, _msg_id(msg_id) {
	//先发送id, 转为网络字节序
	short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
	memcpy(_data, &msg_id_host, HEAD_ID_LEN);
	//转为网络字节序
	short max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
	memcpy(_data + HEAD_ID_LEN, &max_len_host, HEAD_DATA_LEN);
	memcpy(_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
}

/********************************************************************************************************/
//接收节点类构造函数	完成初始化即可
RecvNode::RecvNode(short max_len, short msg_id) :
	MsgNode(max_len),_msg_id(msg_id) 
{
}

/********************************************************************************************************/
