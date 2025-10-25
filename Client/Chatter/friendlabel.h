#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H

#include <QFrame>

/******************************************************************************
 *
 * @file       friendlabel.h
 * @brief      Function: 添加好友界面的单项 friendlabel 栏
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
namespace Ui {
class FriendLabel;
}

class FriendLabel : public QFrame
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel();

    void SetText(QString text); //设置文本
    int Width();                //获取宽度
    int Height();               //获取高度
    QString Text();             //获取文本

signals:
    void sig_close(QString text);

private slots:
    void slot_close(); //其他界面告诉他关闭时处理

private:
    Ui::FriendLabel *ui;

    QString _text; //文本
    int _width;    //宽度
    int _height;   //高度
};

#endif // FRIENDLABEL_H
