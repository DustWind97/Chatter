#ifndef ADDUSERITEM_H
#define ADDUSERITEM_H

#include <QWidget>
#include "listitembase.h"

/******************************************************************************
 *
 * @file       adduseritem.h
 * @brief      Function: 添加好友Item
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
namespace Ui {
class AddUserItem;
}

class AddUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem();

public:
    virtual QSize sizeHint() const override;

private:
    Ui::AddUserItem *ui;
};

#endif // ADDUSERITEM_H
