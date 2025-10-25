#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include "listitembase.h"
#include "userdata.h"

/******************************************************************************
 *
 * @file       chatuserwid.h
 * @brief      Function: ListItem基类，可以其他Item的地方都使用
 *
 * @author     DustWind丶
 * @date       2025/04/21
 * @history
 *****************************************************************************/
namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();

public:
    //void SetInfo(QString name, QString head, QString msg);//设置用户信息
    void SetInfo(std::shared_ptr<UserInfo> user_info);      //设置用户信息（传入UserInfo）
    void SetInfo(std::shared_ptr<FriendInfo> friend_info);  //设置用户信息（传入FriendInfo）
    void ShowRedPoint(bool bshow = false);                  //展示红点，默认为不展示
    std::shared_ptr<UserInfo> GetUserInfo();                //获取用户信息
    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs);//更新最后一条聊天记录


    virtual QSize sizeHint() const override;

private:
    Ui::ChatUserWid *ui;

    std::shared_ptr<UserInfo> _user_info; //用户信息
};

#endif // CHATUSERWID_H
