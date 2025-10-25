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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
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

//Defer类
class Defer {
public:
	//构造，接收一个lambda表达式或函数指针
	Defer(std::function<void()> func):func_(func) {}
	//析构，执行传入的函数
	~Defer() {
		func_;
	}
private:
	std::function<void()> func_;
};

//#define CODEPREFIX "code_"
#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"

/*
class ConfigMgr;
extern ConfigMgr gCfgMgr;
*/