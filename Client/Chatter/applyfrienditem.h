#ifndef APPLYFRIENDITEM_H
#define APPLYFRIENDITEM_H

#include <QWidget>
#include <memory>
#include "listitembase.h"
#include "userdata.h"

/******************************************************************************
 *
 * @file       applyfrienditem.h
 * @brief      Function: 申请好友页面Item
 *
 * @author     DustWind丶
 * @date       2025/06/05
 * @history
 *****************************************************************************/
namespace Ui {
class ApplyFriendItem;
}

class ApplyFriendItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ApplyFriendItem(QWidget *parent = nullptr);
    ~ApplyFriendItem();

    void SetInfo(std::shared_ptr<ApplyInfo> apply_info);
    void ShowAddBtn(bool bshow);
    int GetUid();//获取Uid

    virtual QSize sizeHint() const override;

signals:
    void sig_auth_friend(std::shared_ptr<ApplyInfo> apply_info);//点击添加按钮后会发送此信号

private:
    Ui::ApplyFriendItem *ui;
    std::shared_ptr<ApplyInfo> _apply_info;//申请信息
    bool _added;//是否添加
};

#endif // APPLYFRIENDITEM_H
