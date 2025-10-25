#include "applyfriendpage.h"
#include "ui_applyfriendpage.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QRandomGenerator>
#include "applyfrienditem.h"
#include "authenfriend.h"
#include "tcpmgr.h"
#include "usermgr.h"

ApplyFriendPage::ApplyFriendPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplyFriendPage)
{
    ui->setupUi(this);

    connect(ui->apply_friend_list, &ApplyFriendList::sig_show_search, this, &ApplyFriendPage::sig_show_search);

    //登录时加载申请列表
    loadApplyList();
    //接受tcp传递的authrsp信号处理
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ApplyFriendPage::slot_auth_rsp);
}

ApplyFriendPage::~ApplyFriendPage()
{
    delete ui;
}

/*测试代码************************************************************************************************************/
//登录后，添加新申请
void ApplyFriendPage::AddNewApply(std::shared_ptr<AddFriendApply> apply)
{
    //先模拟头像随机，以后头像资源增加资源服务器后再显示
    // int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    // int head_i = randomValue % heads.size();

    auto* apply_item = new ApplyFriendItem();
    auto apply_info = std::make_shared<ApplyInfo>(apply->_from_uid,
                                                  apply->_name,
                                                  apply->_desc,
                                                  apply->_icon,         /*heads[head_i]*/
                                                  apply->_name, 0, 0);
    apply_item->SetInfo(apply_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(apply_item->sizeHint());
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    ui->apply_friend_list->insertItem(0,item);
    ui->apply_friend_list->setItemWidget(item, apply_item);
    apply_item->ShowAddBtn(true);
    _unauth_items[apply->_from_uid] = apply_item;//将新添加项放入到待认证map里面

    //收到点击添加好友按钮的认证信号
    connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
        qDebug() << "待同意好友信息: name " << apply_info->_name << " uid "
                 << apply_info->_uid << " icon " << apply_info->_icon << " desc is " << apply_info->_desc;

        auto* authFriend = new AuthenFriend(this);
        authFriend->setModal(true);
        authFriend->SetApplyInfo(apply_info);
        authFriend->show();
    });
}

void ApplyFriendPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //???是否需要重绘
    return QWidget::paintEvent(event);
}

//加载申请列表
void ApplyFriendPage::loadApplyList()
{
    //添加好友申请
    auto apply_list = UserMgr::GetInstance()->GetApplyList();
    for(auto &apply: apply_list){
        // int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        // int head_i = randomValue % heads.size();
        auto* apply_item = new ApplyFriendItem();
        apply->SetIcon(apply->_icon/*heads[head_i]*/);
        apply_item->SetInfo(apply);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->insertItem(0,item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        if(apply->_status){
            apply_item->ShowAddBtn(false);
        }else{
            apply_item->ShowAddBtn(true);
            auto uid = apply_item->GetUid();
            _unauth_items[uid] = apply_item;
        }

        //收到好友申请认证信号
        connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
            auto* authFriend = new AuthenFriend(this);
            authFriend->setModal(true);
            authFriend->SetApplyInfo(apply_info);
            authFriend->show();
        });
    }

    // 模拟假数据，创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 8; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % msgs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();

        auto *apply_item = new ApplyFriendItem();
        auto apply = std::make_shared<ApplyInfo>(0, names[name_i], msgs[str_i],
                                                 heads[head_i], names[name_i], 0, 1);
        apply_item->SetInfo(apply);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->addItem(item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        //收到好友申请认证信号
        connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info){
            auto *authFriend =  new AuthenFriend(this);
            authFriend->setModal(true);
            authFriend->SetApplyInfo(apply_info);
            authFriend->show();
        });
    }
}

void ApplyFriendPage::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    int uid = auth_rsp->_uid;
    auto find_iter = _unauth_items.find(uid);
    //未找到，直接返回
    if (find_iter == _unauth_items.end())//未找到
    {
        return;
    }

    //找到，就将按钮隐藏
    find_iter->second->ShowAddBtn(false);

    //查询迭代器，看是否还有未认证的好友，如果没有就不再显示红点（放界面刷新里面）
    //显示红点
}
