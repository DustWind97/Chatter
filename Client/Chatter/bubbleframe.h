#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H

#include <QFrame>
#include <QHBoxLayout>
#include "global.h"

/******************************************************************************
 *
 * @file       bubbleframe.h
 * @brief      Function: 气泡框
 *
 * @author     DustWind丶
 * @date       2025/04/21
 * @history
 *****************************************************************************/
class BubbleFrame:public QFrame
{
    Q_OBJECT

public:
    BubbleFrame(ChatRole role, QWidget *parent = nullptr);
    ~BubbleFrame();

    void setMargin(int margin);//用不上
    void setWidget(QWidget *wid);//添加TextEdit等聊天内容使用

protected:
    void paintEvent(QPaintEvent *event) override;//override?

private:
    QHBoxLayout *m_pHLayout;
    ChatRole m_role;
    int m_margin;
};

#endif // BUBBLEFRAME_H
