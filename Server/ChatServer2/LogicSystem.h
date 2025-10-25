#pragma once
#include "Singleton.h"
#include <queue>
#include <thread>
#include "CSession.h"
//#include <queue>
#include <map>
#include <functional>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>
#include "data.h"

class CServer;//此处的CServer为什么？
//定义回调函数后续聊天相关都在这个回调里面处理
typedef std::function<void(std::shared_ptr<CSession>, 
					  const short& msg_id, 
					  const std::string& msg_data)> FunCallBack;

class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(std::shared_ptr <LogicNode> msg);//消息投递到逻辑队列
private:
	LogicSystem();
	void DealMsg();
	void RegisterCallBacks();//注册等消息回调函数
	//1.登陆请求回调
	void LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string & msg_data);
	//2.搜索用户回调
	void SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	//3.好友申请回调
	void AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	//4.认证好友申请
	void AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	//5.处理文本聊天消息
	void DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	
	//判断是否纯数字字符串
	bool isPureDigit(const std::string& str);
	//获取用户信息（通过uid）
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	//获取用户信息（通过名字）
	void GetUserByName(std::string name, Json::Value& rtvalue);
	//获取用户基本信息
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	//获取好友申请信息
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	//获取好友列表
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);

	std::thread _worker_thread;//工作线程
	std::queue<std::shared_ptr<LogicNode>> _msg_que;//消息队列
	std::mutex _mutex;//线程锁
	std::condition_variable _consume;//消耗条件变量		生产者和消费者模型
	bool _b_stop;//是否停止工作线程
	std::map<short, FunCallBack> _fun_callbacks;//回调函数集
};

