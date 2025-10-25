#include "contactuserlist.h"
#include <QTimer>
#include <QCoreApplication>
#include <QRandomGenerator>
#include "listitembase.h"
#include "userdata.h"
#include "usermgr.h"
#include "tcpmgr.h"
#include "grouptipitem.h"//组别item
#include "contactuseritem.h"//联系人item

//有参构造
ContactUserList::ContactUserList(QWidget *parent)://:QListWidget(parent)
    _load_pending(false),
    _add_friend_item(nullptr)
{
    Q_UNUSED(parent);

    //隐藏横向和纵向的滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);

    //模拟从数据库或者后端传输过来的数据,进行列表加载
    addContactUserList();

    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &ContactUserList::slot_item_clicked);

    //连接对端同意认证后通知的信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend,
            this, &ContactUserList::slot_add_auth_friend);

    //连接自己点击同意认证后界面刷新
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp,
            this, &ContactUserList::slot_auth_rsp);
}

//析构
ContactUserList::~ContactUserList()
{

}

//显示红点
void ContactUserList::ShowRedPoint(bool bshow)
{
    _add_friend_item->ShowRedPoint(bshow);
}

//事件过滤器
bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量

        if (maxScrollValue - currentValue <= 0) {
            //判断是否加载完成
            auto b_loaded = UserMgr::GetInstance()->IsLoadContactFin();
            if(b_loaded){
                return true;
            }
            //加载阻塞
            if(_load_pending){
                return true;
            }

            // 滚动到底部，加载新的联系人
            qDebug()<<"load more contact user";
            _load_pending = true;

            //滑动延迟定时器，设定100ms延迟
            QTimer::singleShot(100, [this](){
                _load_pending = false;
                QCoreApplication::quit(); // 完成后退出应用程序
            });


            //发送信号通知聊天界面加载更多聊天内容，滚动到底部发送信号加载后续的联系人信息
            emit sig_loading_contact_user();
        }
        return true; // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);
}

//添加联系人列表
/*测试代码************************************************************************************************************/
void ContactUserList::addContactUserList()
{
    //1.新的好友Item栏
    GroupTipItem * groupTip = new GroupTipItem();
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);//不允许被选中

    //创建自定义Item
    _add_friend_item = new ContactUserItem();
    _add_friend_item->setObjectName("new_friend_item");//设置item名字（id，样式表中可查找）
    _add_friend_item->SetInfo(0,tr("新的朋友"),":/res/add_friend.png");
    _add_friend_item->SetItemType(ListItemType::APPLY_FRIEND_ITEM);

    //用自定义的Item替换原Item
    QListWidgetItem *add_item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    add_item->setSizeHint(_add_friend_item->sizeHint());
    this->addItem(add_item);
    this->setItemWidget(add_item, _add_friend_item);
    //默认设置新的朋友申请条目被选中
    this->setCurrentItem(add_item);

    //2.好友列表Item栏
    GroupTipItem * groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("联系人"));
    _group_item = new QListWidgetItem;
    _group_item->setSizeHint(groupCon->sizeHint());
    this->addItem(_group_item);
    this->setItemWidget(_group_item, groupCon);
    _group_item->setFlags(_group_item->flags() & ~Qt::ItemIsSelectable);

    //加载后端发送过来的好友列表
    auto con_list = UserMgr::GetInstance()->GetContactListPerPage();
    for(auto & con_ele : con_list){
        auto *con_user_wid = new ContactUserItem();
        con_user_wid->SetInfo(con_ele->_uid,con_ele->_name, con_ele->_icon);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
    UserMgr::GetInstance()->UpdateContactLoadedCount();

    /*测试代码模拟列表************************************************************************************************************/
    // 模拟列表， 创建QListWidgetItem，并设置自定义的widget 13个联系人
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        //int msgs_i = randomValue % msgs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();

        auto *con_user_wid = new ContactUserItem();
        con_user_wid->SetInfo(0,names[name_i], heads[head_i]);

        //用自定义的Item替换原Item
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
    /*测试代码******************************************************************************************************************/
}

//每个item被点击时的处理
void ContactUserList::slot_item_clicked(QListWidgetItem *item)
{
    // 获取自定义widget对象
    QWidget *widget = this->itemWidget(item);
    //获取失败处理
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    //对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    //转换失败处理
    if(!customItem)
    {
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    ListItemType itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM ||
       itemType == ListItemType::GROUP_TIP_ITEM)
    {
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    //申请好友的 item
    if(itemType == ListItemType::APPLY_FRIEND_ITEM)
    {

        // 创建对话框，提示用户
        qDebug()<< "apply friend item clicked ";
        //跳转到好友申请界面
        emit sig_switch_apply_friend_page();
        return;
    }

    //联系人 item
    if(itemType == ListItemType::CONTACT_USER_ITEM)
    {
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";

        ContactUserItem* con_item = qobject_cast<ContactUserItem*>(customItem);
        auto user_info = con_item->GetInfo();
        //跳转到好友信息界面
        emit sig_switch_friend_info_page(user_info);
        return;
    }
}

void ContactUserList::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info)
{
    qDebug() << "slot add auth friend ";
    // 检查是否已经添加好友
    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if(isFriend){
        return;
    }
    // // 在 groupitem 之后插入新项
    // int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    // int str_i = randomValue%strs.size();
    // int head_i = randomValue%heads.size();

    auto *con_user_wid = new ContactUserItem();
    con_user_wid->SetInfo(auth_info);
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取 groupitem 的索引
    int index = this->row(_group_item);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);

    this->setItemWidget(item, con_user_wid);
}

void ContactUserList::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    qDebug() << "slot_auth_rsp uid is " << auth_rsp->_uid
             << " name is " << auth_rsp->_name << " nick is " << auth_rsp->_nick;

    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if(isFriend){
        return;
    }
    // // 在 groupitem 之后插入新项
    // int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    // //int msgs_i = randomValue % msgs.size();
    // int head_i = randomValue % heads.size();
    // //int name_i = randomValue % names.size();

    auto *con_user_wid = new ContactUserItem();
    con_user_wid->SetInfo(auth_rsp->_uid, auth_rsp->_name, auth_rsp->_icon/*heads[head_i]*/);
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());

    // 获取 groupitem 的索引
    int index = this->row(_group_item);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);

    this->setItemWidget(item, con_user_wid);


    // //添加到联系人列表(在ContactUserList中处理)
    // //todo...
    // auto* contact_user_item = new ContactUserItem();
    // contact_user_item->SetInfo(auth_rsp);
    // QListWidgetItem* contact_item = new QListWidgetItem;
    // contact_item->setSizeHint(contact_item->sizeHint());
    // ui->contact_user_list->insertItem(0, item);
    // ui->contact_user_list->setItemWidget(contact_item, contact_user_item);

}
