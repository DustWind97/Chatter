#ifndef TIMERBTN_H
#define TIMERBTN_H
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>

/******************************************************************************
 *
 * @file       timerbtn.h
 * @brief      Function: 重写QPushButton，实现定时按钮
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class TimerBtn : public QPushButton
{
public:
    TimerBtn(QWidget *parent = nullptr);//点击按钮会触发事件，因此需要写一个释放事件
    ~ TimerBtn();

protected:
    void mouseReleaseEvent(QMouseEvent *e) override;//重写释放事件，重写mouseReleaseEvent

private:
    QTimer *_timer; // 计时器
    int _countdown; // 计数器倒计时，在此期间按钮不可操作
};

#endif // TIMERBTN_H
