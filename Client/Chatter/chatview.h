#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QEvent>
#include <QPaintEvent>

/******************************************************************************
 *
 * @file       chatview.h
 * @brief      Function: 聊天消息区域界面纯代码写UI
 *
 * @author     DustWind丶
 * @date       2025/04/21
 * @history
 *****************************************************************************/
class ChatView : public QWidget
{
    Q_OBJECT

public:
    ChatView(QWidget *parent = nullptr);
    ~ChatView();

    void prependChatItem(QWidget *item);                 //头部插入单条聊天记录
    void insertChatItem(QWidget *before, QWidget *item); //从中间插入单条聊天记录
    void appendChatItem(QWidget *item);                  //从末尾插入单条聊天记录
    void removeAllItem();                                //移除所有条目

private:
    void initStyleSheet();//初始化样式表

public:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;//事件过滤器
protected:
    virtual void paintEvent(QPaintEvent *event) override;//绘制事件

signals:

private slots:
    void onVScrollBarMoved(int min,int max);

private:
    QVBoxLayout *m_pVl;         //上下滑动为垂直布局
    QScrollArea *m_pScrollArea; //滚动区域
    bool isAppended;            //是否已经插入添加，实际是一个锁机制
};

#endif // CHATVIEW_H
