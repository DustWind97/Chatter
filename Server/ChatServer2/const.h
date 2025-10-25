#pragma once
//此文件用来管理项目所有所需头文件，ps：涉及到许多类复用

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <functional> //回调
#include <map>
#include <unordered_map>
//JSON库所需头文件
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/filesystem.hpp>//为了跨平台移植，用boost带的文件处理头文件
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <hiredis.h>
#include <cassert>
//MySQL所需头文件
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>
#include <thread>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes
{
	SUCCESS = 0,//成功
	Error_Json = 1001,//Json解析错误
	RPCFailed = 1002,//RPC请求错误
	VerifyExpired = 1003, //验证码过期
	VerifyCodeErr = 1004, //验证码错误
	UserExist = 1005,     //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码不匹配失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
};

// Defer类
class Defer {
public:
	//构造，接收一个lambda表达式或函数指针
	Defer(std::function<void()> func) : func_(func) {}
	//析构，执行传入的函数
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};



//最大包长度
#define MAX_LENGTH  1024*2 
//头部总长度
#define HEAD_TOTAL_LEN 4
//头部id长度
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE  10000
#define MAX_SENDQUE 1000

enum MSG_IDS {
	MSG_CHAT_LOGIN				= 1005,	// 用户聊天登陆
	MSG_CHAT_LOGIN_RSP			= 1006, // 用户聊天登陆回包
	ID_SEARCH_USER_REQ			= 1007, // 用户搜索请求
	ID_SEARCH_USER_RSP			= 1008, // 搜索用户回包
	ID_ADD_FRIEND_REQ			= 1009, // 申请添加好友请求
	ID_ADD_FRIEND_RSP			= 1010, // 申请添加好友回复
	ID_NOTIFY_ADD_FRIEND_REQ	= 1011, // 通知用户添加好友申请
	ID_AUTH_FRIEND_REQ			= 1013, // 认证好友请求
	ID_AUTH_FRIEND_RSP			= 1014, // 认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ	= 1015, // 通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ		= 1017, // 文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP		= 1018, // 文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 通知用户文本聊天信息

	ID_NOTIFY_OFF_LINE_REQ		= 1021, // 通知用户下线
	ID_HEART_BEAT_REQ			= 1023, // 心跳请求
	ID_HEART_BEAT_RSP			= 1024, // 心跳回复
};

//Redis相关前缀定义
#define USERIPPREFIX  "uip_"		//user IP
#define USERTOKENPREFIX  "utoken_"	//user token
#define IPCOUNTPREFIX  "ipcount_"	//IP计数
#define LOGIN_COUNT  "logincount"	//登陆计数
#define USER_BASE_INFO "ubaseinfo_"	//uid查询前缀
#define NAME_INFO  "nameinfo_"		//name查询前缀

#define LOCK_PREFIX "lock_"
#define USER_SESSION_PREFIX "usession_"
#define LOCK_COUNT "lockcount"
//分布式锁的持有时间
#define LOCK_TIME_OUT 10
//分布式锁的重试时间
#define ACQUIRE_TIME_OUT 5
