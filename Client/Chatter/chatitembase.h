#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "global.h"

/******************************************************************************
 *
 * @file       chatitembase.h
 * @brief      Function: ChatItem基类，其他地方可以调用
 *
 * @author     DustWind丶
 * @date       2025/04/21
 * @history
 *****************************************************************************/
class BubbleFrame;//聊天气泡

class ChatItemBase:public QWidget
{
    Q_OBJECT

public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);
    ~ChatItemBase();

    void SetUserName(const QString &name); //设置用户名
    void SetUserIcon(const QPixmap &icon); //设置用户头像
    void SetWidget(QWidget *wid);          //设置Widget

private:
    ChatRole m_role;       //角色
    QLabel*  m_pNameLabel; //名字标签
    QLabel*  m_pIconLabel; //头像标签
    QWidget* m_pBubble;    //气泡框

};

#endif // CHATITEMBASE_H
