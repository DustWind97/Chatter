#include "clickedlabel.h"
#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget *parent):QLabel(parent),_curstate(ClickLabelState::Normal)
{
    //初始为闭眼状态
}

ClickedLabel::~ClickedLabel()
{

}

//鼠标按下事件
void ClickedLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLabelState::Normal){
            //qDebug()<<"clicked , change to selected hover: "<< _selected_hover;
            _curstate = ClickLabelState::Selected;
            setProperty("state",_selected_hover);
            repolish(this);
            update();

        }else{
            //qDebug()<<"clicked , change to normal hover: "<< _normal_hover;
            _curstate = ClickLabelState::Normal;
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(), _curstate);
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

// 处理鼠标悬停进入事件
void ClickedLabel::enterEvent(QEnterEvent *event)
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

    QLabel::enterEvent(event);
}

// 处理鼠标悬停离开事件
void ClickedLabel::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(_curstate == ClickLabelState::Normal){
        //qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();

    }else{
        //qDebug()<<"leave , change to normal hover: "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::SetState(QString normal, QString normal_hover, QString normal_press,
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

ClickLabelState ClickedLabel::GetCurState()
{
    return _curstate;
}

bool ClickedLabel::SetCurState(ClickLabelState state)
{
    _curstate = state;
    if(_curstate == ClickLabelState::Normal)
    {
        setProperty("state", _normal);
        repolish(this);
    }
    else if(_curstate == ClickLabelState::Selected)
    {
        setProperty("state", _selected);
        repolish(this);
    }
    return true;
}

void ClickedLabel::ResetNormalState()
{
    //重置就是将状态设置为未选中状态
    _curstate = ClickLabelState::Normal;
    setProperty("state", _normal);
    repolish(this);
}


