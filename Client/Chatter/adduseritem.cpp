#include "adduseritem.h"
#include "ui_adduseritem.h"

AddUserItem::AddUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::AddUserItem)
{
    ui->setupUi(this);

    SetItemType(ListItemType::ADD_USER_TIP_ITEM);//设置item类型为添加用户乐行
}

AddUserItem::~AddUserItem()
{
    delete ui;
}

QSize AddUserItem::sizeHint() const
{
    return QSize(250,70);//返回窗口尺寸
}
