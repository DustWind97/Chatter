#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include <QWidget>
#include "userdata.h"
#include <memory>
#include <QJsonArray>
#include <unordered_map>
#include "applyfrienditem.h"

/******************************************************************************
 *
 * @file       applyfriendpage.h
 * @brief      Function: 申请好友页面
 *
 * @author     DustWind丶
 * @date       2025/06/05
 * @history
 *****************************************************************************/
namespace Ui {
class ApplyFriendPage;
}

class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();

    void AddNewApply(std::shared_ptr<AddFriendApply> apply);//添加新的朋友申请

protected:
    virtual void paintEvent(QPaintEvent *event) override;//重绘事件

private:
    void loadApplyList();//加载好友申请列表(通常为服务器方发来的数据用于APP初次渲染)

signals:
    void sig_show_search(bool);//展示搜素框信号

public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);//接收好友申请之后触发.

private:
    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem*> _unauth_items;//无序图，管理还未认证信息条目
};

#endif // APPLYFRIENDPAGE_H
