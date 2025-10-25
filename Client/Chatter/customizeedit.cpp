#include "customizeedit.h"

CustomizeEdit::CustomizeEdit(QWidget *parent):QLineEdit(parent), _max_len(0)
{
    //连接QlineEdit内容改变信号
    connect(this, &QLineEdit::textChanged, this, &CustomizeEdit::limitTextLenght );
}

CustomizeEdit::~CustomizeEdit()
{

}

void CustomizeEdit::SetMaxLength(int maxlen)
{
    _max_len = maxlen;
}

void CustomizeEdit::focusOutEvent(QFocusEvent *event)
{
    //执行失去焦点逻辑
    //调用基类的focusOutEvent方法保证基类行为正常
    QLineEdit::focusOutEvent(event);

    //发送失去焦点信号
    emit sig_focus_out();
}

void CustomizeEdit::limitTextLenght(QString text)
{
    if( _max_len <= 0)
    {
        return;
    }

    QByteArray byteArray = text.toUtf8();//对于汉字长度不一致，因此转成utf8的编码格式

    if( byteArray.size() >= _max_len )
    {
        byteArray = byteArray.left(_max_len);
        this->setText(QString::fromUtf8(byteArray));
    }
}
