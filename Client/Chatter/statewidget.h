#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include "global.h"
#include <QEvent>
#include <QEnterEvent>

/******************************************************************************
 *
 * @file       statewidget.h
 * @brief      Function: 设置SideBar的状态 StateWidget
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class StateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StateWidget(QWidget *parent = nullptr);
    ~StateWidget();

    void SetState(QString normal="", QString normal_hover="", QString normal_press="",
                  QString select="", QString select_hover="", QString select_press="");

    ClickLabelState GetCurState();
    void ClearState();
    void SetSelected(bool bselected);
    void AddRedPoint();
    void ShowRedPoint(bool show=true);

protected:
    void paintEvent(QPaintEvent* event) override;

    virtual void enterEvent(QEnterEvent* event) override;        // 重写进入事件
    virtual void leaveEvent(QEvent* event) override;             // 重写离开事件
    virtual void mousePressEvent(QMouseEvent *event) override;   // 重写鼠标按下事件
    virtual void mouseReleaseEvent(QMouseEvent *event) override; // 重写鼠标释放事件

signals:
    void clicked(void);

public slots:

private:
    QString _normal;           // 普通状态
    QString _normal_hover;     // 普通 悬浮状态
    QString _normal_press;     // 普通 按下状态

    QString _selected;         // 选中状态
    QString _selected_hover;   // 选中 悬浮状态
    QString _selected_press;   // 选中 按下状态

    ClickLabelState _curstate; // 可点击标签当前状态
    QLabel *_red_point;        // 红点
};

#endif // STATEWIDGET_H
