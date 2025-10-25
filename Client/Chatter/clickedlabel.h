#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>
#include "global.h"

/******************************************************************************
 *
 * @file       clickedlabel.h
 * @brief      Function: 重写QLabel控件
 *
 * @author     DustWind丶
 * @date       2025/04/19
 * @history
 *****************************************************************************/
class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent=nullptr);
    ~ClickedLabel();

    void SetState(QString normal = "", QString normal_hover = "", QString normal_press = "",
                  QString select = "", QString select_hover = "", QString select_press = ""); //设置初始状态

    ClickLabelState GetCurState();           //获取可点击标签当前状态
    bool SetCurState(ClickLabelState state); //设置可点击标签当前状态（_curstate成员变量）
    void ResetNormalState();                 //重置为普通状态

protected:
    virtual void enterEvent(QEnterEvent *event) override;   //重写进入事件
    virtual void leaveEvent(QEvent *event) override;        //重写离开事件
    virtual void mousePressEvent(QMouseEvent *ev) override; //重写鼠标按下事件

signals:
    void clicked(QString str, ClickLabelState state); //标签点击信号

private:
    QString _normal;         //普通状态
    QString _normal_hover;   //普通 悬浮状态
    QString _normal_press;   //普通 按下状态

    QString _selected;       //选中状态
    QString _selected_hover; //选中 悬浮状态
    QString _selected_press; //选中 按下状态

    ClickLabelState _curstate; //可点击标签当前状态
};

#endif // CLICKEDLABEL_H
