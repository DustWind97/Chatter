#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H

#include <QWidget>
#include "listitembase.h"

/******************************************************************************
 *
 * @file       grouptipitem.h
 * @brief      Function: 组别item
 *
 * @author     DustWind丶
 * @date       2025/06/04
 * @history
 *****************************************************************************/
namespace Ui {
class GroupTipItem;
}

class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();

    virtual QSize sizeHint() const override;
    void SetGroupTip(QString str);

private:
    Ui::GroupTipItem *ui;

    QString _tip;
};

#endif // GROUPTIPITEM_H
