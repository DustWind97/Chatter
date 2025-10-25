#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include <QRandomGenerator>
#include "chatuserwid.h"
#include <QPixmap>
#include "contactuseritem.h"
#include "tcpmgr.h"
#include "usermgr.h"
// #include "chatitembase.h"
// #include "textbubble.h"
// #include "picturebubble.h"
// #include "messagetextedit.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
    ,_mode(ChatUIMode::ChatMode)
    ,_state(ChatUIMode::ChatMode)
    ,_b_loading(false)
    ,_cur_chat_uid(0)
    ,_last_widget(nullptr)
{
    ui->setupUi(this);

    // ui->add_btn->SetState("normal","hover","press");
    // ui->search_edit->SetMaxLength(15);
    // ui->search_edit->setAlignment(Qt::AlignCenter);

    // //创建一个搜索动作
    // QAction *searchAction = new QAction(ui->search_edit);
    // searchAction->setIcon(QIcon(":/res/search.png"));
    // ui->search_edit->addAction(searchAction,QLineEdit::LeadingPosition);//将搜素动作添加到LineEdit头部
    // ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    // //创建一个清除动作
    // QAction *clearAction = new QAction(ui->search_edit);
    // clearAction->setIcon(QIcon(":/res/close_transparent.png"));//开始不显示清除图标
    // ui->search_edit->addAction(clearAction,QLineEdit::TrailingPosition);//将清除动作添加在LineEdit末尾

    // //当实际清除时更改为清除的图标
    // connect(ui->search_edit,&QLineEdit::textChanged,[clearAction](const QString &text){
    //     if(!text.isEmpty())//当输入框内容不为空时，显示清除图标
    //     {
    //         clearAction->setIcon(QIcon(":/res/close_search.png"));
    //     }
    //     else//当输入框内容为空时，显示透明图标
    //     {
    //         clearAction->setIcon(QIcon(":/res/close_transparent.png"));
    //     }
    // });

    // //执行清除动作
    // connect(clearAction, &QAction::triggered,[this,clearAction](){
    //     ui->search_edit->clear();//清除文本内容
    //     clearAction->setIcon(QIcon(":/res/close_transparent.png"));//清除内容后将清楚动作显示透明图标
    //     ui->search_edit->clearFocus();//搜素框失去焦点

    //     ShowSearch(false);//当清除按钮被按下，则不显示搜素框
    // });

    // //设置登陆者信息
    // QString login_name = UserMgr::GetInstance()->GetName();
    // ui->login_name_label->setText("登陆者:" + login_name);

    // //设置中心部件为聊天页
    // ui->stackedWidget->setCurrentWidget(ui->chat_page);

    // //设置搜索框
    // ui->search_edit->SetMaxLength(15);//设置搜索框长度
    // ShowSearch(false);//默认情况下隐藏搜素框

    // //连接加载列表
    // connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);

    // //模拟从数据库或者后端传输过来的数据,进行列表加载
    // AddChatUserList();

    // //用户图标加载（后面是动态加载的）
    // QPixmap pixmap(":/res/head_1.jpg");
    // //ui->side_head_label->setPixmap(pixmap);//将图片设置到 QLabel 上
    // QPixmap scaledPixmap = pixmap.scaled(ui->side_head_label->size(), Qt::KeepAspectRatio);//将图片缩放到 QLabel 大小
    // ui->side_head_label->setPixmap(scaledPixmap);//将缩放后的图片设置到 QLabel 上
    // ui->side_head_label->setScaledContents(true);//设置 QLabel 自动缩放图片内容以适应大小

    // //side_chat_label设置属性
    // //ui->side_chat_label->setProperty("state","normal");
    // ui->side_chat_label->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    // //ui->side_contact_label->setProperty("state","normal");
    // ui->side_contact_label->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    // AddLabelGroup(ui->side_chat_label);
    // AddLabelGroup(ui->side_contact_label);

    // //连接side_chat_label点击信号
    // connect(ui->side_chat_label, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    // //连接side_contact_label点击信号
    // connect(ui->side_contact_label, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    // //连接搜索框输入变化
    // connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);

    // //检测鼠标点击位置判断是否清空搜索框
    // this->installEventFilter(this);


    // //连接加载聊天的信号和槽
    // connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
    // //连接加载联系人的信号和槽
    // connect(ui->contact_user_list, &ContactUserList::sig_loading_contact_user, this, &ChatDialog::slot_loading_contact_user);


    // //设置默认进入程序聊天选中状态
    // ui->side_chat_label->SetSelected(true);

    // //为searchlist 设置 searchedit
    // ui->search_list->SetSearchEdit(ui->search_edit);

    // //设置选中条目
    // SetSelectChatItem();//？？？？？？？
    // //更新聊天界面信息
    // SetSelectChatPage();


    // //连接申请添加好友申请信号
    // connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_friend_apply, this, &ChatDialog::slot_apply_friend);

    // //连接认证添加好友信号
    // connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ChatDialog::slot_add_auth_friend);

    // //连接自己认证回复信号
    // connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ChatDialog::slot_auth_rsp);

    // //从搜索框跳转聊天项
    // connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);

    // //从好友信息页跳转聊天项
    // connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item_from_infopage);

    // //连接点击联系人item发出的信号和好友信息展示槽函数
    // connect(ui->contact_user_list, &ContactUserList::sig_switch_friend_info_page, this, &ChatDialog::slot_friend_info_page);

    // //连接联系人页面点击好友申请条目信号
    // connect(ui->contact_user_list, &ContactUserList::sig_switch_apply_friend_page,
    //         this, &ChatDialog::slot_switch_apply_friend_page);

    // //连接聊天列表点击信号
    // connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);


    ui->add_btn->SetState("normal","hover","press");
    ui->add_btn->setProperty("state","normal");

    //创建一个搜索动作并设置图标
    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/res/search.png"));
    ui->search_edit->addAction(searchAction,QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    // 创建一个清除动作并设置图标
    QAction *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/res/close_transparent.png"));
    // 初始时不显示清除图标
    // 将清除动作添加到LineEdit的末尾位置
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    // 当需要显示清除图标时，更改为实际的清除图标
    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/res/close_search.png"));
        } else {
            clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 文本为空时，切换回透明图标
        }
    });

    // 连接清除动作的触发信号到槽函数，用于清除文本
    connect(clearAction, &QAction::triggered, [this, clearAction]() {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 清除文本后，切换回透明图标
        ui->search_edit->clearFocus();
        //清除按钮被按下则不显示搜索框
        ShowSearch(false);
    });



    //搜索框限制长度
    ui->search_edit->SetMaxLength(15);
    //搜索框文本位置居中
    ui->search_edit->setAlignment(Qt::AlignCenter);

    //连接加载信号和槽
    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);

    //渲染聊天用户列表
    AddChatUserList();

    //设置登陆者信息
    QString login_name = UserMgr::GetInstance()->GetUserInfo()->_name;
    ui->login_name_label->setText("登陆者:" + login_name);
    //加载自己头像
    QString head_icon = UserMgr::GetInstance()->GetUserInfo()->_icon;
    QPixmap pixmap(head_icon); // 加载图片
    QPixmap scaledPixmap = pixmap.scaled( ui->side_head_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); //将图片缩放到label的大小
    ui->side_head_label->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->side_head_label->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小

    ui->side_chat_label->setProperty("state","normal");
    ui->side_chat_label->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_label->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    //ui->side_settings_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    AddLabelGroup(ui->side_chat_label);
    AddLabelGroup(ui->side_contact_label);
    //AddLBGroup(ui->side_settings_lb);

    connect(ui->side_chat_label, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_label, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    //connect(ui->side_settings_label, &StateWidget::clicked, this, &ChatDialog::slot_side_setting);

    //链接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);

    ShowSearch(false);

    //检测鼠标点击位置判断是否要清空搜索框
    this->installEventFilter(this); // 安装事件过滤器

    //设置聊天label选中状态
    ui->side_chat_label->SetSelected(true);
    //设置选中条目
    SetSelectChatItem();
    //更新聊天界面信息
    SetSelectChatPage();

    //连接加载联系人的信号和槽函数
    connect(ui->contact_user_list, &ContactUserList::sig_loading_contact_user,
            this, &ChatDialog::slot_loading_contact_user);

    //连接联系人页面点击好友申请条目的信号
    connect(ui->contact_user_list, &ContactUserList::sig_switch_apply_friend_page,
            this,&ChatDialog::slot_switch_apply_friend_page);

    //连接清除搜索框操作
    connect(ui->friend_apply_page, &ApplyFriendPage::sig_show_search, this, &ChatDialog::slot_show_search);

    //为searchlist 设置search edit
    ui->search_list->SetSearchEdit(ui->search_edit);

    //连接申请添加好友信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_friend_apply, this, &ChatDialog::slot_apply_friend);

    //连接认证添加好友信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ChatDialog::slot_add_auth_friend);

    //链接自己认证回复信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this,
            &ChatDialog::slot_auth_rsp);

    //连接点击联系人item发出的信号和用户信息展示槽函数
    connect(ui->contact_user_list, &ContactUserList::sig_switch_friend_info_page,
            this,&ChatDialog::slot_friend_info_page);

    //设置中心部件为chatpage
    ui->stackedWidget->setCurrentWidget(ui->chat_page);


    //连接searchlist跳转聊天信号
    connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);

    //连接好友信息界面发送的点击事件
    connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item, this,
            &ChatDialog::slot_jump_chat_item_from_infopage);

    //连接聊天列表点击信号
    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);

    //发送消息缓存
    connect(ui->chat_page, &ChatPage::sig_append_send_chat_msg, this, &ChatDialog::slot_append_send_chat_msg);

    //接收消息缓存，连接对端消息
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_text_chat_msg, this, &ChatDialog::slot_text_chat_msg);

}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if(bsearch)//处于Search状态搜素模式
    {
        //展示搜素列表
        ui->chat_user_list->hide();
        ui->contact_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    }
    else if(_state == ChatUIMode::ChatMode)//处于Chat状态聊天模式
    {
        //展示聊天列表
        ui->chat_user_list->show();
        ui->contact_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
    }
    else if(_state == ChatUIMode::ContactMode)//处于Contact状态联系模式
    {
        //展示联系列表
        ui->chat_user_list->hide();
        ui->contact_user_list->show();
        ui->search_list->hide();
        _mode = ChatUIMode::ContactMode;
    }
}

//添加到label组
void ChatDialog::AddLabelGroup(StateWidget *label)
{
    _label_list.push_back(label);
}


//添加聊天用户表
void ChatDialog::AddChatUserList()
{
    //先按照好友列表加载聊天记录，等以后客户端实现聊天记录数据库之后再按照最后信息排序
    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto find_iter = _chat_items_added.find(friend_ele->_uid);
            if(find_iter != _chat_items_added.end()){
                continue;
            }
            auto *chat_user_wid = new ChatUserWid();
            auto user_info = std::make_shared<UserInfo>(friend_ele);
            chat_user_wid->SetInfo(user_info);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_items_added.insert(friend_ele->_uid, item);
        }

        //更新已加载条目
        UserMgr::GetInstance()->UpdateChatLoadedCount();
    }


    /*如下为测试代码***********************************************************************************************************/
    // 创建QListWidgetItem，并设置自定义的widget
    // for(int i = 0; i < 7; i++){
    //     int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    //     int str_i = randomValue % msgs.size();//下标随机取余
    //     int head_i = randomValue % heads.size();
    //     int name_i = randomValue % names.size();

    //     auto *chat_user_wid = new ChatUserWid();
    //     auto user_info = std::make_shared<UserInfo>(0, names[name_i], names[name_i], heads[head_i], 0, msgs[str_i]);//绑定参数
    //     chat_user_wid->SetInfo(user_info);

    //     QListWidgetItem *item = new QListWidgetItem;
    //     //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    //     item->setSizeHint(chat_user_wid->sizeHint());
    //     ui->chat_user_list->addItem(item);
    //     ui->chat_user_list->setItemWidget(item, chat_user_wid);
    // }
    /*如上为测试代码***********************************************************************************************************/
}

//事件过滤器
bool ChatDialog::eventFilter(QObject *wanted, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)//鼠标点击事件
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }

    return QDialog::eventFilter(wanted, event);
}

//处理全局鼠标按下事件
void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    //实现点击位置判断和处理逻辑
    //先判断是否处于搜素模式，如果不处于搜索模式则直接返回
    if(_mode != ChatUIMode::SearchMode)
    {
        return;
    }

    //将鼠标点击位置转化为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->search_list->mapFromGlobal(event->globalPosition()).toPoint();//显式地将QPointF转化为QPoint类型
    //判断点击位置是否在聊天列表范围内
    if(!ui->search_list->rect().contains(posInSearchList))
    {
        //如果不在聊天列表中，清空搜索框
        ui->search_edit->clear();
        ShowSearch(false);//并且不显示搜素结果
    }
}

//清除标签状态
void ChatDialog::ClearLabelState(StateWidget *label)
{
    //遍历标签列表，找到对应的label并清空除开label以外的其他所有label
    for(auto &elem : _label_list)
    {
        if(elem == label)
        {
            continue;
        }
        elem->ClearState();
    }
}

//加载聊天用户
void ChatDialog::slot_loading_chat_user()
{
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDialog *loadingDialog = new LoadingDialog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    LoadMoreChatUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

//加载联系人用户
void ChatDialog::slot_loading_contact_user()
{
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDialog *loadingDialog = new LoadingDialog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    LoadMoreContactUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

//点击sidebar的chat标签
void ChatDialog::slot_side_chat()
{
    qDebug()<< "receive side chat clicked";

    ClearLabelState(ui->side_chat_label);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);//正在聊天的列表
    //展示Chat模式界面
    _state = ChatUIMode::ChatMode;
    ShowSearch(false);
}

//点击联系人标签
void ChatDialog::slot_side_contact()
{
    qDebug()<< "receive side contact clicked";

    ClearLabelState(ui->side_contact_label);
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page); //好友列表

    //展示历史聊天页
    ClearLabelState(ui->side_contact_label);
    if(_last_widget == nullptr){
        ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
        _last_widget = ui->friend_apply_page;
    }else{
        ui->stackedWidget->setCurrentWidget(_last_widget);
    }

    //展示Contact模式界面
    _state = ChatUIMode::ContactMode;
    ShowSearch(false);

}

//搜索框内容变化
void ChatDialog::slot_text_changed(const QString &str)
{
    qDebug()<< "receive slot text changed str is" << str;
    if(!str.isEmpty())//搜索框不为空
    {
        ShowSearch(true);//展示搜素结果界面
    }
}

void ChatDialog::slot_show_search(bool b_show)
{
    ShowSearch(b_show);
}

//申请添加好友回包处理
void ChatDialog::slot_apply_friend(std::shared_ptr<AddFriendApply> apply_info)
{
    bool b_already = UserMgr::GetInstance()->AlreadyApply(apply_info->_from_uid);//查询是否申请列表已经存在
    if(b_already){
        return;
    }

    UserMgr::GetInstance()->AddApplyList(std::make_shared<ApplyInfo>(apply_info));//不存在则添加到列表

    //显示红点
    ui->side_contact_label->ShowRedPoint(true);
    ui->contact_user_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply_info);
}

//对端发送好友认证信息，本端处理
void ChatDialog::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info) {
    qDebug() << "receive slot_add_auth__friend uid is " << auth_info->_uid
             << " name is " << auth_info->_name << " nick is " << auth_info->_nick;

    //判断如果已经是好友则跳过
    auto bfriend = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if(bfriend){
        return;
    }

    //如果不是好友，则添加好友到本地管理的 _friend_map 中
    UserMgr::GetInstance()->AddFriend(auth_info);

    //添加到聊天列表
    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_info->_uid, item);
}

//本端发送认证请求回复处理
void ChatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    // qDebug() << "receive slot_auth_rsp uid is " << auth_rsp->_uid
    //          << " name is " << auth_rsp->_name << " nick is " << auth_rsp->_nick << "icon is " << auth_rsp->_icon;

    //判断如果已经是好友则跳过
    auto bfriend = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if(bfriend){
        qDebug() << "已经是好友";
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_rsp);

    //添加到聊天列表
    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_rsp);//??????????参数不匹配
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_rsp->_uid, item);


    //本端认证通过，取消红点显示
    ui->side_contact_label->ShowRedPoint(false);
    ui->contact_user_list->ShowRedPoint(false);
}

//是好友，跳转聊天页面
void ChatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> search_info)
{
    qDebug() << "slot jump chat item ";
    auto find_iter = _chat_items_added.find(search_info->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item , uid is " << search_info->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_label->SetSelected(true);
        SetSelectChatItem(search_info->_uid);
        //更新聊天界面信息
        SetSelectChatPage(search_info->_uid);
        slot_side_chat();
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(search_info);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    _chat_items_added.insert(search_info->_uid, item);

    ui->side_chat_label->SetSelected(true);
    SetSelectChatItem(search_info->_uid);
    //更新聊天界面信息
    SetSelectChatPage(search_info->_uid);
    slot_side_chat();
}

//通过好友信息页跳转好友聊天页
void ChatDialog::slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info)
{
    // 先看是否在聊天列表中
    qDebug() << "slot jump chat item ";
    auto find_iter = _chat_items_added.find(user_info->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item , uid is " << user_info->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_label->SetSelected(true);
        SetSelectChatItem(user_info->_uid);
        //更新聊天界面信息
        SetSelectChatPage(user_info->_uid);
        slot_side_chat();
        return;
    }

    //如果不在，则创建新的插入listwidget
    auto* chat_user_wid = new ChatUserWid();
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(user_info->_uid, item);
    ui->side_chat_label->SetSelected(true);

    SetSelectChatItem(user_info->_uid);
    //更新聊天界面信息
    SetSelectChatPage(user_info->_uid);
    slot_side_chat();
}

//跳转好友信息页
void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info)
{
    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}

//切换好友认证页
void ChatDialog::slot_switch_apply_friend_page()
{
    _last_widget = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

//聊天项点击
void ChatDialog::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = ui->chat_user_list->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM ||
       itemType == ListItemType::GROUP_TIP_ITEM)
    {
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::CHAT_USER_ITEM)
    {
        // 创建对话框，提示用户
        qDebug()<< "chat user item clicked ";

        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto user_info = chat_wid->GetUserInfo();
        //跳转到聊天界面
        ui->chat_page->SetUserInfo(user_info);
        _cur_chat_uid = user_info->_uid;
        return;
    }
}

void ChatDialog::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata)
{
    if (_cur_chat_uid == 0) {
        return;
    }

    auto find_iter = _chat_items_added.find(_cur_chat_uid);
    if (find_iter == _chat_items_added.end()) {
        return;
    }

    //转为widget
    QWidget* widget = ui->chat_user_list->itemWidget(find_iter.value());
    if (!widget) {
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if (itemType == CHAT_USER_ITEM) {
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if (!con_item) {
            return;
        }

        //设置信息
        auto user_info = con_item->GetUserInfo();
        user_info->_chat_msgs.push_back(msgdata);
        std::vector<std::shared_ptr<TextChatData>> msg_vec;
        msg_vec.push_back(msgdata);

        UserMgr::GetInstance()->AppendFriendChatMsg(_cur_chat_uid, msg_vec);
        return;
    }
}

//处理对端发送过来的消息
void ChatDialog::slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg)
{
    auto find_iter = _chat_items_added.find(msg->_from_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "set chat item msg, uid is " << msg->_from_uid;
        QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
        auto chat_wid = qobject_cast<ChatUserWid*>(widget);
        if(!chat_wid){
            return;
        }

        //更新用户
        chat_wid->updateLastMsg(msg->_chat_msgs);
        //更新当前聊天页面记录
        UpdateChatMsg(msg->_chat_msgs);
        UserMgr::GetInstance()->AppendFriendChatMsg(msg->_from_uid,msg->_chat_msgs);
        return;
    }

    //如果没找到，则创建新的插入listwidget

    ChatUserWid* chat_user_wid = new ChatUserWid();
    //查询好友信息
    std::shared_ptr<FriendInfo> fi_ptr = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
    chat_user_wid->SetInfo(fi_ptr);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    chat_user_wid->updateLastMsg(msg->_chat_msgs);
    UserMgr::GetInstance()->AppendFriendChatMsg(msg->_from_uid,msg->_chat_msgs);
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(msg->_from_uid, item);
}

//设置选中聊天项
void ChatDialog::SetSelectChatItem(int uid)
{
    if(ui->chat_user_list->count() <= 0){
        return;
    }

    if(uid == 0){
        ui->chat_user_list->setCurrentRow(0);
        QListWidgetItem *firstItem = ui->chat_user_list->item(0);
        if(!firstItem){
            return;
        }

        //转为widget
        QWidget *widget = ui->chat_user_list->itemWidget(firstItem);
        if(!widget){
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item){
            return;
        }

        _cur_chat_uid = con_item->GetUserInfo()->_uid;

        return;
    }

    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        qDebug() << "uid " <<uid<< " not found, set curent row 0";
        ui->chat_user_list->setCurrentRow(0);
        return;
    }

    ui->chat_user_list->setCurrentItem(find_iter.value());

    _cur_chat_uid = uid;
}

//设置选中聊天页面
void ChatDialog::SetSelectChatPage(int uid)
{
    if( ui->chat_user_list->count() <= 0){
        return;
    }

    if (uid == 0) {
        auto item = ui->chat_user_list->item(0);
        //转为widget
        QWidget* widget = ui->chat_user_list->itemWidget(item);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if (!con_item) {
            return;
        }

        //设置信息
        std::shared_ptr<UserInfo> user_info = con_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }

    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        return;
    }

    //转为widget
    QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
    if(!widget){
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == CHAT_USER_ITEM){
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if(!con_item){
            return;
        }

        //设置信息
        auto user_info = con_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);

        return;
    }
}

void ChatDialog::LoadMoreChatUser()
{
    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto find_iter = _chat_items_added.find(friend_ele->_uid);
            if(find_iter != _chat_items_added.end()){
                continue;
            }
            auto *chat_user_wid = new ChatUserWid();
            auto user_info = std::make_shared<UserInfo>(friend_ele);
            chat_user_wid->SetInfo(user_info);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_items_added.insert(friend_ele->_uid, item);
        }

        //更新已加载条目
        UserMgr::GetInstance()->UpdateChatLoadedCount();
    }
}

void ChatDialog::LoadMoreContactUser()
{
    auto friend_list = UserMgr::GetInstance()->GetContactListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto *chat_user_wid = new ContactUserItem();
            chat_user_wid->SetInfo(friend_ele->_uid,friend_ele->_name,
                                   friend_ele->_icon);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->contact_user_list->addItem(item);
            ui->contact_user_list->setItemWidget(item, chat_user_wid);
        }

        //更新已加载条目
        UserMgr::GetInstance()->UpdateContactLoadedCount();
    }
}

//更新聊天信息
void ChatDialog::UpdateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgs)
{
    for(auto& msg : msgs)
    {
        if(msg->_from_uid != _cur_chat_uid)//不是当前聊天对象,不做处理(将消息缓存到本地成员中)
        {
            break;
        }

        //是当前聊天对象，则需要在界面展示出来
        ui->chat_page->AppendChatMsg(msg);
    }
}
