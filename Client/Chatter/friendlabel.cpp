#include "friendlabel.h"
#include "ui_friendlabel.h"
#include <QFontMetrics>


FriendLabel::FriendLabel(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FriendLabel)
{
    ui->setupUi(this);

    ui->close_label->SetState("normal","hover","pressed",
                              "selected","selected_hover","selected_pressed");
    connect(ui->close_label, &ClickedLabel::clicked, this, &FriendLabel::slot_close);

}

FriendLabel::~FriendLabel()
{
    delete ui;
}

void FriendLabel::SetText(QString text)
{
    _text = text;
    ui->tip_label->setText(text);
    ui->tip_label->adjustSize();

    QFontMetrics fontMetrics(ui->tip_label->font());//获取 QLabel 字体信息
    //获取文本宽度
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        int textWidth = fontMetrics.width(text);
    #else
        int textWidth = fontMetrics.horizontalAdvance(text);
    #endif
    //获取文本高度
    int textHeight = fontMetrics.height();

    qDebug()<< " ui->tip_lb.width() is " << ui->tip_label->width();
    qDebug()<< " ui->close_lb->width() is " << ui->close_label->width();
    qDebug()<< " textWidth is " << textWidth;

    this->setFixedWidth(ui->tip_label->width() + ui->close_label->width() + 5);
    this->setFixedHeight(textHeight + 2);
    _width = this->width();
    _height = this->height();

}

int FriendLabel::Width()
{
    return _width;
}

int FriendLabel::Height()
{
    return _height;
}

QString FriendLabel::Text()
{
    return _text;
}

void FriendLabel::slot_close()
{
    emit sig_close(_text);
}
