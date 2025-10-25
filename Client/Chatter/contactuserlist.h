#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H

#include <QListWidget>
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>
#include <memory>
#include "userdata.h"
#include <QLabel>

/******************************************************************************
 *
 * @file       contactuserlist.h
 * @brief      Function: 重写 con_user_wid 的 item
 *
 * @author     DustWind丶
 * @date       2025/06/04
 * @history
 *****************************************************************************/
class ContactUserItem;//单项item

class ContactUserList : public QListWidget
{
    Q_OBJECT

public:
    ContactUserList(QWidget *parent = nullptr);
    ~ContactUserList();

    void AddRedPoint();
    void ShowRedPoint(bool bshow=true);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void addContactUserList();

signals:
    void sig_loading_contact_user();//加载联系人
    void sig_switch_apply_friend_page();//切换到申请好友界面
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);//切换到好友信息界面

private slots:
    void slot_item_clicked(QListWidgetItem* item);//每个item被点击时的处理
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);//TCP数据处理之添加好友验证（对面同意之后的回包）
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);//别人添加我方之后刷新本地列表并回包

private:
    bool _load_pending;//载入阻塞
    ContactUserItem *_add_friend_item;//list里面的一个item
    QListWidgetItem *_group_item;
};

#endif // CONTACTUSERLIST_H
