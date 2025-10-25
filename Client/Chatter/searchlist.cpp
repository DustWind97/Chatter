#include "searchlist.h"
#include<QScrollBar>
#include "adduseritem.h"
//#include "invaliditem.h"
#include "findsuccessdialog.h"
#include "tcpmgr.h"
#include "customizeedit.h"
#include "findfaildialog.h"
#include "loadingdialog.h"
#include "userdata.h"
#include "usermgr.h"
#include "userdata.h"

SearchList::SearchList(QWidget *parent):
    QListWidget(parent),
    _send_pending(false)/*初始未阻塞*/,
    _find_dlg(nullptr),
    _search_edit(nullptr)
{
    //关闭垂直和水平滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //安装事件过滤器
    this->viewport()->installEventFilter(this);

    //连接Item点击信号
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目，收到服务器信息再初始化列表
    addTipItem();
    //连接搜索条目信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

SearchList::~SearchList()
{

}

//清除查找好友弹出窗
void SearchList::CloseFindDialog()
{
    if(_find_dlg)
    {
        _find_dlg->hide();
        _find_dlg = nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit = edit;
}

bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave)
        {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}

void SearchList::waitPending(bool pending)
{
    //发送过程中加载一个等待界面
    if(pending)
    {
        _loadingDialog = new LoadingDialog(this);
        _loadingDialog->setModal(true);//模态
        _loadingDialog->show();
        _send_pending = pending;
    }
    else
    {
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _send_pending = pending;
    }
}

//添加item提示
void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;

    //qDebug()<<"chat_user_wid sizeHint is " << ui->chat_user_wid->sizeHint();

    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);//设置不可选中


    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem();
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

//判断字符串是否为纯数字	用于区分名字和uid
bool SearchList::isPureDigit(const std::string& str)
{
    for (char c : str)
    {
        if (!std::isdigit(c))
        {
            return false;//不是纯数字
        }
    }
    return true;//是纯数字
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget = this->itemWidget(item);//获取item项并转换为QWidget
    if(!widget)
    {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    //对自定义widget进行操作，将 item 转化为基类 ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem)
    {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    //1.如果是不可点击条目
    if(itemType == ListItemType::INVALID_ITEM)
    {
        qDebug() << "slot invalid item clicked";
        return;
    }
    //2.如果添加好友条目
    if(itemType == ListItemType::ADD_USER_TIP_ITEM)
    {
        //如果发送阻塞
        if(_send_pending)//阻塞于此处，等待查询数据库
        {
            return;
        }
        // if(!_search_edit)//搜索框为空
        // {
        //     return;
        // }

        //qDebug()<<"waitPending..... " ;
        waitPending(true);//在等待过程中会有一个加载界面

        CustomizeEdit* search_edit = dynamic_cast<CustomizeEdit*>(_search_edit);//父子之间转换
        QJsonObject jsonObj;
        QString query_str = search_edit->text();//这里可以是给昵称，也可以是uid        后面需要改

        jsonObj["querystr"] = query_str;
        QJsonDocument doc(jsonObj);
        QString jsonString = doc.toJson(QJsonDocument::Indented);

        //发送tcp请求给 ChatServer
        emit TcpMgr::GetInstance().get()->sig_send_data(ReqId::ID_SEARCH_USER_REQ, jsonString.toUtf8());

        return;
    }

    //清除弹出窗
    CloseFindDialog();
}

//处理服务器过来的回包
void SearchList::slot_user_search(std::shared_ptr<SearchInfo> search_info)
{
    waitPending(false);//在等待完成，关闭此加载界面
    if (search_info == nullptr)//出现一些问题便弹出失败对话框
    {
        _find_dlg = std::make_shared<FindFailDialog>(this);
    }
    else//查到用户
    {
        //如果是自己，暂且先直接返回，以后看逻辑扩充
        auto self_uid = UserMgr::GetInstance()->GetUid();
        if (search_info->_uid == self_uid)
        {
            // to do ......
            qDebug()<<"查询的用户为自己" ;
            return;
        }
        //此处分两种情况，一种是搜多到已经是自己的朋友了，一种是未添加好友
        //1.查找是否已经是好友，查询本地缓存
        bool bExist = UserMgr::GetInstance()->CheckFriendById(search_info->_uid);
        if(bExist){
            //此处处理已经添加的好友，实现页面跳转
            //跳转到聊天界面指定的item中
            qDebug()<<"双方已经是好友" ;
            emit sig_jump_chat_item(search_info);
            return;
        }
        //2.处理添加的好友
        qDebug()<<"查到好友信息："<< "name" <<  search_info->_name  << "" << search_info->_uid << " ....";
        _find_dlg = std::make_shared<FindSuccessDialog>(this);
        std::dynamic_pointer_cast<FindSuccessDialog>(_find_dlg)->SetSearchInfo(search_info);

    }
    _find_dlg->show();
}
