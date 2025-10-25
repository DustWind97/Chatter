#include "friendinfopage.h"
#include "ui_friendinfopage.h"
#include <QDebug>

FriendInfoPage::FriendInfoPage(QWidget *parent)
    :QWidget(parent)
    ,ui(new Ui::FriendInfoPage)
    ,_user_info(nullptr)
{
    ui->setupUi(this);

    ui->msg_chat->SetState("normal","hover","press");   //设置文本聊天按钮初始状态
    ui->voice_chat->SetState("normal","hover","press"); //设置音频聊天按钮初始状态
    ui->video_chat->SetState("normal","hover","press"); //设置视频聊天按钮初始状态
}

FriendInfoPage::~FriendInfoPage()
{
    delete ui;
}

void FriendInfoPage::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    // 加载图片
    QPixmap pixmap(user_info->_icon);

    // 设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    ui->name_label->setText(user_info->_name);
    ui->nick_label->setText(user_info->_nick);
    ui->back_label->setText(user_info->_nick);
}

//文本聊天按钮
void FriendInfoPage::on_msg_chat_clicked()
{
    qDebug() << "msg chat btn clicked";
    emit sig_jump_chat_item(_user_info);
}

//音频聊天按钮
void FriendInfoPage::on_voice_chat_clicked()
{

}

//视频聊天按钮
void FriendInfoPage::on_video_chat_clicked()
{

}

