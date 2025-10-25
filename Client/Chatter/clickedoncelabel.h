#ifndef CLICKEDONCELABEL_H
#define CLICKEDONCELABEL_H

#include <QLabel>
#include <QMouseEvent>

/******************************************************************************
 *
 * @file       clickedoncelabel.h
 * @brief      Function: 重写QLabel控件，一次点击
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class ClickedOnceLabel : public QLabel
{
    Q_OBJECT

public:
    ClickedOnceLabel(QWidget *parent = nullptr);
    ~ClickedOnceLabel();

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void clicked(QString text);
};

#endif // CLICKEDONCELABEL_H
