#include "findsuccessdialog.h"
#include "ui_findsuccessdialog.h"
#include <QDir>
#include "applyfriend.h"

FindSuccessDialog::FindSuccessDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindSuccessDialog)
    , _parent(parent)
{
    ui->setupUi(this);

    //设置对话框标题
    setWindowTitle("添加");
    //隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    //获取当前程序路径,后续是要跟服务器交互的
    //程序路径
    QString app_path = QCoreApplication::applicationDirPath();
    //头像路径
    QString pix_path = QDir::toNativeSeparators(app_path + QDir::separator() + "static" + QDir::separator() + "head_1.jpg");

    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_label->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);//图片自适应等比缩放
    ui->head_label->setPixmap(head_pix);
    ui->add_friend_btn->SetState("normal", "hover", "press");
    this->setModal(true);
}

FindSuccessDialog::~FindSuccessDialog()
{
    qDebug() << "FindSuccessDialog was destructed";
    delete ui;
}

void FindSuccessDialog::SetSearchInfo(std::shared_ptr<SearchInfo> search_info)
{
    ui->name_label->setText(search_info->_name);
    _search_info = search_info;
}

//添加好友界面交互
void FindSuccessDialog::on_add_friend_btn_clicked()
{
    this->hide();
    auto applyFriend = new ApplyFriend(_parent);
    applyFriend->SetSearchInfo(_search_info);
    applyFriend->setModal(true);//设置为模态窗口，处理完此窗口才能执行其他窗口
    applyFriend->show();
}

