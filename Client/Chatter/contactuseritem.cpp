#include "contactuseritem.h"
#include "ui_contactuseritem.h"

ContactUserItem::ContactUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ContactUserItem)
{
    ui->setupUi(this);

    SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();//将红点图层放在最上层，Z坐标
    ShowRedPoint(false);
}

ContactUserItem::~ContactUserItem()
{
    delete ui;
}

QSize ContactUserItem::sizeHint() const
{
    return QSize(250,70);//返回自定义尺寸
}

//设置信息  AuthInfo
void ContactUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    _user_info = std::make_shared<UserInfo>(auth_info);

    //加载图片
    QPixmap pixmap(_user_info->_icon);

    //设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio/*等比缩放*/, Qt::SmoothTransformation/*平滑*/));
    ui->icon_label->setScaledContents(true);
    ui->user_name_label->setText(_user_info->_name);
}

//设置信息  AuthRsp
void ContactUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp)
{
    _user_info = std::make_shared<UserInfo>(auth_rsp);

    //加载图片
    QPixmap pixmap(_user_info->_icon);

    //设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);
    ui->user_name_label->setText(_user_info->_name);
}

//设置信息 uid,name,icon
void ContactUserItem::SetInfo(int uid, QString name, QString icon)
{
    _user_info = std::make_shared<UserInfo>(uid, name, icon);

    //加载图片
    QPixmap pixmap(_user_info->_icon);

    //设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);
    ui->user_name_label->setText(_user_info->_name);
}

//显示红点
void ContactUserItem::ShowRedPoint(bool bshow)
{
    if(bshow)
    {
        ui->red_point->show();
    }
    else
    {
        ui->red_point->hide();
    }
}

std::shared_ptr<UserInfo> ContactUserItem::GetInfo()
{
    return _user_info;
}
