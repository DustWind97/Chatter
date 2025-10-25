#include "clickedoncelabel.h"

ClickedOnceLabel::ClickedOnceLabel(QWidget *parent):QLabel(parent)
{
    setCursor(Qt::PointingHandCursor);//设置手指Cursor
}

ClickedOnceLabel::~ClickedOnceLabel()
{

}

void ClickedOnceLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit clicked(this->text());
        return;
    }

    // 调用基类的 mouseReleaseEvent 以保证正常的事件处理
    QLabel::mouseReleaseEvent(event);
}
