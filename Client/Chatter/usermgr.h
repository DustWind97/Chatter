#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include <memory>
#include <singleton.h>
#include "userdata.h"
#include <vector>

/******************************************************************************
 *
 * @file       usermgr.h
 * @brief      Function: 用户管理类(管理本地内存中用户信息数据)
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class UserMgr : public QObject, public Singleton<UserMgr>, public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT

public:
    friend class Singleton<UserMgr>;
    ~UserMgr();

private:
    UserMgr();

public:
    void SetToken(QString token);                             // 设置Token
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);    // 设置用户信息
    int  GetUid();                                            // 获取uid
    QString GetName();                                        // 获取名字
    std::shared_ptr<UserInfo> GetUserInfo();                  // 获取用户信息
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();   // 获取申请列表
    bool CheckFriendById(int uid);                            // 查询好友是否已经添加
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);        // 添加好友，别人添加我方之后刷新本地列表并回包
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);      // 添加好友，对面同意之后的回包
    std::shared_ptr<FriendInfo> GetFriendById(int uid);       // 通过uid获取好友信息
    bool AlreadyApply(int uid);                               // 查询申请人是否已在申请列表
    void AddApplyList(std::shared_ptr<ApplyInfo> apply_info); // 添加申请列表
    void AppendApplyList(QJsonArray array);                   // 追加申请列表
    void AppendFriendList(QJsonArray array);                  // 追加好友列表
    void AppendFriendChatMsg(int friend_id, std::vector<std::shared_ptr<TextChatData>> msgs); //追加好友聊天信息

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();    // 获取聊天页分页列表
    bool IsLoadChatFin();                                             // 是否加载聊天页分页完成
    void UpdateChatLoadedCount();                                     // 更新加载计数
    std::vector<std::shared_ptr<FriendInfo>> GetContactListPerPage(); // 获取每页联系人列表
    bool IsLoadContactFin();                                          // 是否加载联系人分页完成
    void UpdateContactLoadedCount();                                  // 更新联系人分页计数


private:
    QString _token;                                        // Token
    std::shared_ptr<UserInfo> _user_info;                  // 用户信息
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;   // 缓存好友申请列表
    std::vector<std::shared_ptr<FriendInfo>> _friend_list; // 缓存好友列表
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;    // 缓存好友列表（以图的方式存储）
    size_t _chat_loaded;                                   // 已加载聊天人
    size_t _contact_loaded;                                // 已加载联系人
};

#endif // USERMGR_H
