#ifndef CLICKEDBTN_H
#define CLICKEDBTN_H

#include <QPushButton>

/******************************************************************************
 *
 * @file       clickedbtn.h
 * @brief      Function: 重写QPushButton控件
 *
 * @author     DustWind丶
 * @date       2025/04/19
 * @history
 *****************************************************************************/
class ClickedBtn:public QPushButton
{
    Q_OBJECT
public:
    ClickedBtn(QWidget * parent= nullptr) ;
    ~ClickedBtn();

    void SetState(QString normal = "", QString hover = "", QString press = ""); //设置初始状态

protected:
    virtual void enterEvent(QEnterEvent *event) override;        //重写进入事件
    virtual void leaveEvent(QEvent *event) override;             //重写离开事件
    virtual void mousePressEvent(QMouseEvent *event) override;   //重写鼠标按下事件
    virtual void mouseReleaseEvent(QMouseEvent *event) override; //重写鼠标释放事件

private:
    QString _normal; //普通状态
    QString _hover;  //悬浮状态
    QString _press;  //按下状态
};

#endif // CLICKEDBTN_H
