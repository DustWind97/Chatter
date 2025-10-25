#include "statewidget.h"
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>

StateWidget::StateWidget(QWidget *parent) : QWidget(parent), _curstate(ClickLabelState::Normal)
{
    setCursor(Qt::PointingHandCursor);//设置为“手形”光标
    //设置红点
    AddRedPoint();//当需要红点的时候只需要将成员变量置为真
}

StateWidget::~StateWidget()
{

}

//设置状态
void StateWidget::SetState(QString normal, QString normal_hover, QString normal_press,
                           QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = normal_hover;
    _normal_press = normal_press;

    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    setProperty("state",normal);
    repolish(this);
}

//获取cursor状态
ClickLabelState StateWidget::GetCurState()
{
    return _curstate;
}

//清空状态(即设置为未选中状态)
void StateWidget::ClearState()
{
    _curstate = ClickLabelState::Normal;
    setProperty("state",_normal);
    repolish(this);
    update();
}

//设置选中状态
void StateWidget::SetSelected(bool bselected)
{
    //选中状态
    if(bselected)
    {
        _curstate = ClickLabelState::Selected;
        setProperty("state",_selected);
        repolish(this);
        update();
        return;
    }

    //未选中状态（默认）
    _curstate = ClickLabelState::Normal;
    setProperty("state",_normal);
    repolish(this);
    update();
    return;
}

//添加红点
void StateWidget::AddRedPoint()
{
    //添加红点示意图
    _red_point = new QLabel();
    _red_point->setObjectName("red_point");

    //创建一个垂直布局
    QVBoxLayout *layout = new QVBoxLayout();
    _red_point->setAlignment(Qt::AlignCenter);
    layout->addWidget(_red_point);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);
    _red_point->setVisible(false);//默认设置不可见
}

//显示红点
void StateWidget::ShowRedPoint(bool show)
{
    _red_point->setVisible(show);
}

//重写绘制事件
void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //???是否需要重绘
    return QWidget::paintEvent(event);
}

void StateWidget::mousePressEvent(QMouseEvent *event)
{
    //按下鼠标左键
    if(event->button() == Qt::LeftButton)
    {
        //选中状态
        if(_curstate == ClickLabelState::Selected)
        {
            //qDebug()<<  "mousePressEvent, already to selected press:" << _selected_press;
            //调用基类的 mousePressEvent 保证正常处理事件
            QWidget::mousePressEvent(event);
            return;
        }

        //未选中状态（默认）
        if(_curstate == ClickLabelState::Normal)
        {
            //qDebug()<< "mousePressEvent, change to selected press:" << _selected_press;
            _curstate = ClickLabelState::Selected;
            setProperty("state",_selected_press);
            repolish(this);
            update();
        }
        return;
    }

    //调用基类的 mousePressEvent 保证正常处理事件
    QWidget::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //松开鼠标左键
    if(event->button() == Qt::LeftButton)
    {
        //未选中状态
        if(_curstate == ClickLabelState::Normal)
        {
            //qDebug()<<  "mouseReleaseEvent, already to selected hover:" << _selected_hover;
            //调用基类的mousePressEvent保证正常处理事件
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }
        else//选中状态 //if(_curstate == ClickLabelState::Selected)
        {
            //qDebug()<<  "mouseReleaseEvent, change to selected hover:" << _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked();
        return;
    }

    //调用基类的 mouseReleaseEvent 保证正常处理事件
    QWidget::mouseReleaseEvent(event);
}

void StateWidget::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(_curstate == ClickLabelState::Normal){
        //qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_normal_hover);
        repolish(this);
        update();

    }else{
        //qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }

    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(_curstate == ClickLabelState::Normal)
    {
        //qDebug()<<"leave , change to normal: "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();

    }else{
        //qDebug()<<"leave , change to selected normal: "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }

    QWidget::leaveEvent(event);
}
