#include "chatuserwid.h"
#include "ui_chatuserwid.h"

ChatUserWid::ChatUserWid(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);

    SetItemType(ListItemType::CHAT_USER_ITEM);
    ui->red_point->raise();//
    ShowRedPoint(false);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

QSize ChatUserWid::sizeHint() const
{
    return QSize(250,70);//返回自定义尺寸 宽度250，长度70
}

void ChatUserWid::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;//缓存用户信息

    //设置头像
    //加载图片
    QPixmap pixmap(_user_info->_icon);
    //设置图片自动缩放 平滑等比缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);
    //设置名字
    ui->user_name_label->setText(_user_info->_name);//展示申请人名字
    //设置聊天内容
    ui->user_chat_label->setText(_user_info->_last_msg);
}

void ChatUserWid::SetInfo(std::shared_ptr<FriendInfo> friend_info)
{
    _user_info = std::make_shared<UserInfo>(friend_info);//缓存用户信息

    //设置头像
    //加载图片
    QPixmap pixmap(_user_info->_icon);
    //设置图片自动缩放 平滑等比缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);
    //设置名字
    ui->user_name_label->setText(_user_info->_name);
    //设置聊天内容
    ui->user_chat_label->setText(_user_info->_last_msg);
}

std::shared_ptr<UserInfo> ChatUserWid::GetUserInfo()
{
    return _user_info;
}

void ChatUserWid::updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    QString last_msg = "";
    for(auto& msg : msgs)
    {
        last_msg = msg->_msg_content;
        _user_info->_chat_msgs.push_back(msg);
    }
    _user_info->_last_msg = last_msg;
    ui->user_chat_label->setText(_user_info->_last_msg);
}

//红点显示
void ChatUserWid::ShowRedPoint(bool bshow)
{
    if(bshow){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }
}
