#include "ChatServiceImpl.h"
#include "UserMgr.h"
#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "RedisMgr.h"
#include "MysqlMgr.h"

ChatServiceImpl::ChatServiceImpl()
{
}

ChatServiceImpl::~ChatServiceImpl()
{
}

//1.获取基本信息
bool ChatServiceImpl::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	//优先查redis中查询用户信息
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base) {
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		userinfo->uid = root["uid"].asInt();
		userinfo->name = root["name"].asString();
		userinfo->pwd = root["pwd"].asString();
		userinfo->email = root["email"].asString();
		userinfo->nick = root["nick"].asString();
		userinfo->desc = root["desc"].asString();
		userinfo->sex = root["sex"].asInt();
		userinfo->icon = root["icon"].asString();
		std::cout << "user login uid is  " << userinfo->uid << " name  is "
			<< userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << std::endl;
	}
	else {
		//redis中没有则查询mysql
		//查询数据库
		std::shared_ptr<UserInfo> user_info = nullptr;
		//std::shared_ptr<UserInfo> user_info = std::make_shared<UserInfo>();//上面一行代码也可以写成这样

		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if (user_info == nullptr) {
			return false;
		}

		userinfo = user_info;

		//将数据库内容写入redis缓存
		Json::Value redis_root;
		redis_root["uid"] = uid;
		redis_root["pwd"] = userinfo->pwd;
		redis_root["name"] = userinfo->name;
		redis_root["email"] = userinfo->email;
		redis_root["nick"] = userinfo->nick;
		redis_root["desc"] = userinfo->desc;
		redis_root["sex"] = userinfo->sex;
		redis_root["icon"] = userinfo->icon;
		RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
	}

	return true;
}

//2.通知添加好友 服务器接收对端服务器处理
Status ChatServiceImpl::NotifyAddFriend(::grpc::ServerContext* context, const AddFriendReq* request, AddFriendRsp* reply)
{
	//查找用户是否在本服务器上线
	auto touid = request->touid();
	auto session = UserMgr::GetInstance()->GetSession(touid);

	//默认设置为成功状态
	reply->set_error(ErrorCodes::SUCCESS);
	Defer defer([request, reply]() {
		//reply->set_error(ErrorCodes::SUCCESS);
		reply->set_applyuid(request->applyuid());
		reply->set_touid(request->touid());
		});

	//用户不在内存中则直接返回
	if (session == nullptr) {
		return Status::OK;
	}

	//在内存中则直接发送通知对方
	Json::Value  rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;
	rtvalue["applyuid"] = request->applyuid();
	rtvalue["name"] = request->name();
	rtvalue["desc"] = request->desc();
	rtvalue["icon"] = request->icon();
	rtvalue["sex"] = request->sex();
	rtvalue["nick"] = request->nick();

	std::string return_str = rtvalue.toStyledString();
	session->Send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);//回包通知
	
	return Status::OK;
}

//3.通知认证好友
Status ChatServiceImpl::NotifyAuthFriend(::grpc::ServerContext* context, const AuthFriendReq* request,
	AuthFriendRsp* reply) {
	std::cout << "-------GRPC Server-------" << std::endl;
	//查找用户是否在本服务器
	auto touid = request->touid();
	auto fromuid = request->fromuid();
	auto session = UserMgr::GetInstance()->GetSession(touid);

	//默认设置为成功状态
	reply->set_error(ErrorCodes::SUCCESS);
	Defer defer([request, reply]() {
		//reply->set_error(ErrorCodes::SUCCESS);
		reply->set_fromuid(request->fromuid());
		reply->set_touid(request->touid());
		});

	//用户不在内存中则直接返回
	if (session == nullptr) {
		return Status::OK;
	}

	//在内存中则直接发送通知对方
	Json::Value  rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;
	rtvalue["fromuid"] = request->fromuid();
	rtvalue["touid"] = request->touid();

	std::string base_key = USER_BASE_INFO + std::to_string(fromuid);
	auto user_info = std::make_shared<UserInfo>();
	bool b_info = GetBaseInfo(base_key, fromuid, user_info);//获取到fromuid的用户信息
	if (b_info) {
		rtvalue["name"] = user_info->name;
		rtvalue["nick"] = user_info->nick;
		rtvalue["icon"] = user_info->icon;
		rtvalue["sex"] = user_info->sex;
	}
	else {
		rtvalue["error"] = ErrorCodes::UidInvalid;
	}

	std::string return_str = rtvalue.toStyledString();

	session->Send(return_str, ID_NOTIFY_AUTH_FRIEND_REQ);
	return Status::OK;
}

//4.通知文本聊天消息
Status ChatServiceImpl::NotifyTextChatMsg(::grpc::ServerContext* context,
	const TextChatMsgReq* request, TextChatMsgRsp* reply) 
{
	//查找用户是否在本服务器
	auto touid = request->touid();
	auto session = UserMgr::GetInstance()->GetSession(touid);
	reply->set_error(ErrorCodes::SUCCESS);

	//用户不在内存中则直接返回
	if (session == nullptr) {
		return Status::OK;
	}

	//在本服务器则直接发送通知对方
	Json::Value  rtvalue;
	rtvalue["error"] = ErrorCodes::SUCCESS;
	rtvalue["fromuid"] = request->fromuid();
	rtvalue["touid"] = request->touid();

	//将聊天数据组织为数组
	Json::Value text_array;
	for (auto& msg : request->textmsgs()) {
		Json::Value element;
		element["content"] = msg.msgcontent();
		element["msgid"] = msg.msgid();
		text_array.append(element);
	}
	rtvalue["text_array"] = text_array;

	std::string return_str = rtvalue.toStyledString();

	session->Send(return_str, ID_NOTIFY_TEXT_CHAT_MSG_REQ);
	std::cout << "ID_NOTIFY_TEXT_CHAT_MSG_REQ success" << std::endl;

	return Status::OK;
}