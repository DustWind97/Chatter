#include "picturebubble.h"
#include <QLabel>

#define PIC_MAX_WIDTH 160
#define PIC_MAX_HEIGHT 90

PictureBubble::PictureBubble(ChatRole role, const QPixmap &picture, QWidget *parent)
    :BubbleFrame(role, parent)
{
    QLabel *label = new QLabel();
    label->setScaledContents(true);
    QPixmap pix = picture.scaled(QSize(PIC_MAX_WIDTH, PIC_MAX_HEIGHT), Qt::KeepAspectRatio);//缩放自适应
    label->setPixmap(pix);
    this->setWidget(label);

    int left_margin = this->layout()->contentsMargins().left();
    int right_margin = this->layout()->contentsMargins().right();
    int v_margin = this->layout()->contentsMargins().bottom();
    setFixedSize(pix.width()+left_margin + right_margin, pix.height() + v_margin *2);
}

PictureBubble::~PictureBubble()
{

}
