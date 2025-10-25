#include "timerbtn.h"
#include <QMouseEvent>
#include <QDebug>

TimerBtn::TimerBtn(QWidget *parent)
    :QPushButton(parent)
    ,_countdown(10)
{
    _timer = new QTimer(this);

    connect(_timer, &QTimer::timeout, [this](){
        _countdown--;
        if(_countdown <= 0){
            _timer->stop();
            _countdown = 10;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(_countdown));//将倒计时显示在按钮上
    });
}

TimerBtn::~TimerBtn()
{
    _timer->stop();
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // 在这里处理鼠标左键释放事件
        qDebug() << "MyButton was released!";
        this->setEnabled(false);
        this->setText(QString::number(_countdown));//将倒计时显示在按钮上
        _timer->start(1000);//设定1000毫秒一次检测（1秒）
        emit clicked();//发送已经点击信号，某些高版本Qt不需要（会在释放事件中调用）
    }

    QPushButton::mouseReleaseEvent(e);//调用基类鼠标释放事件以确保正常事件的处理（如点击效果）
}



