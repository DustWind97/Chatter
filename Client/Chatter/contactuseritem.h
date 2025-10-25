#ifndef CONTACTUSERITEM_H
#define CONTACTUSERITEM_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"

/******************************************************************************
 *
 * @file       contactuseritem.h
 * @brief      Function: 联系人item
 *
 * @author     DustWind丶
 * @date       2025/06/04
 * @history
 *****************************************************************************/
namespace Ui {
class ContactUserItem;
}

class ContactUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ContactUserItem(QWidget *parent = nullptr);
    ~ContactUserItem();

    QSize sizeHint() const override;
    void SetInfo(std::shared_ptr<AuthInfo> auth_info); //设置用户信息（通过认证信息）
    void SetInfo(std::shared_ptr<AuthRsp> auth_rsp);   //设置用户信息（通过认证回包）
    void SetInfo(int uid, QString name, QString icon); //设置用户信息（通过基本uid,name,icon设置）
    void ShowRedPoint(bool bshow = false);             //展示红点，默认为不展示
    std::shared_ptr<UserInfo> GetInfo();               //获取用户信息

private:
    Ui::ContactUserItem *ui;

    std::shared_ptr<UserInfo> _user_info;
};

#endif // CONTACTUSERITEM_H
