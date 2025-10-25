#include "applyfrienditem.h"
#include "ui_applyfrienditem.h"

ApplyFriendItem::ApplyFriendItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ApplyFriendItem)
    , _added(false)
{
    ui->setupUi(this);

    SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    ui->apply_add_btn->SetState("normal","hover", "press");
    ui->apply_add_btn->hide();

    //添加好友按钮按下发送信号
    connect(ui->apply_add_btn, &ClickedBtn::clicked,  [this](){
        emit this->sig_auth_friend(_apply_info);
        qDebug() << "认证好友:" << _apply_info->_name << " " << _apply_info->_uid;
    });
}

ApplyFriendItem::~ApplyFriendItem()
{
    delete ui;
}

//设置此Item的内容
void ApplyFriendItem::SetInfo(std::shared_ptr<ApplyInfo> apply_info)
{
    _apply_info = apply_info;//将信息缓存到成员变量

    // 加载图片
    QPixmap pixmap(_apply_info->_icon);
    // 设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    ui->user_name_label->setText(_apply_info->_name);//设置用户名
    ui->user_chat_label->setText(_apply_info->_desc);//设置申请好友简短信息
}

//展示添加好友按钮
void ApplyFriendItem::ShowAddBtn(bool bshow)
{
    //二选一展示[添加好友QPushButton][已经添加好友QLabel]
    if (bshow) {
        ui->apply_add_btn->show();
        ui->already_add_label->hide();
        _added = false;
    }
    else {
        ui->apply_add_btn->hide();
        ui->already_add_label->show();
        _added = true;
    }
}

//获取尺寸
QSize ApplyFriendItem::sizeHint() const
{
    return QSize(250, 80); // 返回自定义的尺寸
}

//获取UID
int ApplyFriendItem::GetUid()
{
    return _apply_info->_uid;
}
