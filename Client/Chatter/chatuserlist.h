#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollEvent>

/******************************************************************************
 *
 * @file       chatuserlist.h
 * @brief      Function: 聊天用户列表
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class ChatUserList:public QListWidget
{
    Q_OBJECT

public:
    ChatUserList(QWidget *parent = nullptr);
    ~ChatUserList();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override; //鼠标滑动的时候才显示边条

signals:
    void sig_loading_chat_user(); //载入聊天用户

private:
    bool _load_pending;
};

#endif // CHATUSERLIST_H
