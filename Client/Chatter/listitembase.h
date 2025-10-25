#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include <QWidget>
#include "global.h"

/******************************************************************************
 *
 * @file       listitembase.h
 * @brief      Function: 重写ListItem为基类，其他布局可以直接使用
 *
 * @author     DustWind丶
 * @date       2025/04/20
 * @history
 *****************************************************************************/
class ListItemBase:public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBase(QWidget *parent = nullptr);//显式构造
    ~ListItemBase();

    void SetItemType(ListItemType itemType); // 设置ItemType
    ListItemType GetItemType();              // 获取ItemType

private:
    ListItemType _itemType;
};

#endif // LISTITEMBASE_H
