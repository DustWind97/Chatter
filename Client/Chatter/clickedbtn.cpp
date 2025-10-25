#include "clickedbtn.h"
#include "global.h"

ClickedBtn::ClickedBtn(QWidget * parent):QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor); //设置光标为小手
    // 设置一句防止被enter事件捕获 无焦点光标
    setFocusPolicy(Qt::NoFocus);
}

ClickedBtn::~ClickedBtn()
{

}

void ClickedBtn::SetState(QString normal, QString normal_hover, QString normal_press)
{
    _normal = normal;
    _hover = normal_hover;
    _press = normal_press;

    setProperty("state",normal);
    repolish(this);
    update();
}

// 处理进入事件
void ClickedBtn::enterEvent(QEnterEvent *event)
{
    //qDebug()<<"enter , change to hover : "<< _hover;
    setProperty("state",_hover);
    repolish(this);
    update();

    QPushButton::enterEvent(event);
}

// 处理离开事件
void ClickedBtn::leaveEvent(QEvent *event)
{
    //qDebug()<<"leave , change to normal : "<< _normal;
    setProperty("state",_normal);
    repolish(this);
    update();

    QPushButton::leaveEvent(event);
}

void ClickedBtn::mousePressEvent(QMouseEvent *event)
{
    //qDebug()<<"mouse press , change to press : "<< _press;
    setProperty("state",_press);
    repolish(this);
    update();

    QPushButton::mousePressEvent(event);
}

void ClickedBtn::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug()<<"mouse release , change to hover : "<< _hover;
    setProperty("state", _hover);
    repolish(this);
    update();

    QPushButton::mouseReleaseEvent(event);
}

