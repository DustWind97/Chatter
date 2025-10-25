#include "UserMgr.h"
#include "CSession.h"
#include "RedisMgr.h"

UserMgr::UserMgr()
{

}

UserMgr::~UserMgr()
{	
	//如果不放心可以析构的时候加下锁
	_uid_to_session.clear(); // 清空会话映射
}

std::shared_ptr<CSession> UserMgr::GetSession(int uid)
{	
	std::lock_guard<std::mutex> lock(_session_mutex); // 确保线程安全
	
	//std::unordered_map<int, std::shared_ptr<CSession>>::iterator iter = _uid_to_session.find(uid);
	auto iter = _uid_to_session.find(uid);//上面为iterator的auto类型推导
	if (iter == _uid_to_session.end())//未找到
	{
		return nullptr;
	}
	//找到返回此连接
	return iter->second;
}

void UserMgr::SetUserSession(int uid, std::shared_ptr<CSession> session)
{
	std::lock_guard<std::mutex> lock(_session_mutex); // 确保线程安全
	_uid_to_session[uid] = session; // 将用户ID和会话绑定
}

void UserMgr::RemoveUserSession(int uid)
{
	std::string uid_str = std::to_string(uid);
	//因为再次登录可能是其他服务器，所以会造成本服务器删除key，其他服务器注册key的情况
	// 有可能其他服务登录，本服删除key造成找不到key的情况
	//RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);

	{
		std::lock_guard<std::mutex> lock(_session_mutex); // 确保线程安全
		_uid_to_session.erase(uid); // 删除用户ID和会话绑定
	}
}
