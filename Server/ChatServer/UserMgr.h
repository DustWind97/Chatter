#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include <mutex>

class CSession;

class UserMgr : public Singleton<UserMgr>
{
	friend class Singleton<UserMgr>;

public:
	
	~UserMgr();
	
	std::shared_ptr<CSession> GetSession(int uid);//获取会话
	void SetUserSession(int uid, std::shared_ptr<CSession> session);//绑定User和Session
	void RemoveUserSession(int uid);//取消User和Session绑定

private:
	UserMgr();
	std::mutex _session_mutex; // 互斥锁，保护会话映射的线程安全
	std::unordered_map<int, std::shared_ptr<CSession>> _uid_to_session; // 用户ID到会话的映射
};

