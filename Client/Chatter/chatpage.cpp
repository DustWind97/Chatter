#include "chatpage.h"
#include "ui_chatpage.h"
#include <QStyleOption>
#include <QPainter>
#include <QJsonArray>
#include <QJsonObject>
#include <QStyleOption>
#include <QUuid>
#include "chatitembase.h"
#include "textbubble.h"
#include "picturebubble.h"
#include "usermgr.h"
#include "tcpmgr.h"
//#include "applyfrienditem.h"

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);

    /*测试用接收按钮，发布时隐藏**********************/
    ui->receive_btn->hide();

    //设置按钮样式
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");

    //设置图标样式
    ui->emoji_label->SetState("normal","hover","press","normal","hover","press");
    ui->file_label->SetState("normal","hover","press","normal","hover","press");

    //连接发送消息快捷键，enter 键和 return 键
    connect(ui->chat_edit, &MessageTextEdit::send, this, &ChatPage::on_send_btn_clicked);

}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    ui->title_label->setText(_user_info->_name);//设置title聊天对象
    ui->chat_data_list->removeAllItem();//清空聊天区域内容

    //加载新增聊天信息
    for (auto & msg : user_info->_chat_msgs) {
        AppendChatMsg(msg);
    }
}

//界面渲染新增聊天信息
void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_info = UserMgr::GetInstance()->GetUserInfo(); //获取本人信息
    ChatRole role;
    //todo... 添加聊天显示
    if (msg->_from_uid == self_info->_uid)//显示对象为自己
    {
        role = ChatRole::Self;
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->SetUserName(self_info->_name);
        pChatItem->SetUserIcon(QPixmap(self_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->SetWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
    else//显示对象为对方
    {
        role = ChatRole::Other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        auto friend_info = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
        if (friend_info == nullptr) {
            return;
        }
        pChatItem->SetUserName(friend_info->_name);
        pChatItem->SetUserIcon(QPixmap(friend_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->SetWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

//清空item项
void ChatPage::clearItems()
{
    ui->chat_data_list->removeAllItem();
}

//发送按钮
void ChatPage::on_send_btn_clicked()
{

    if(_user_info == nullptr)
    {
        qDebug() << "friend_info is empty" ;
        return;
    }

    auto user_info = UserMgr::GetInstance()->GetUserInfo();//获取自己信息
    auto pTextEdit = ui->chat_edit;//获取聊天框内容
    ChatRole role = ChatRole::Self;
    QString userName = user_info->_name;    //用户名
    QString userIcon = user_info->_icon;    //用户头像

    const QVector<MsgInfo> &msgList = pTextEdit->getMsgList();
    QJsonObject textObj;    //待发送消息的JSON对象
    QJsonArray textArray;   //待发送消息的JSON数组
    int text_size = 0;      //累计要发送的消息长度

    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度合规，跳过
        if(msgList[i].content.length() > 1024)//单条消息不超过1kb
        {
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->SetUserName(userName);
        pChatItem->SetUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;

        //1.处理文本消息
        if(type == "text")
        {
            //每一条消息都生成唯一消息uid
            QUuid uuid = QUuid::createUuid();
            QString uuidString = uuid.toString();
            pBubble = new TextBubble(role, msgList[i].content);//新建一个文本气泡框

            //消息长度超过1kb（发送消息长度累计+消息列表中当前消息长度），就先将消息发送给服务器
            if(text_size + msgList[i].content.length() > 1024)
            {
                textObj["fromuid"] = user_info->_uid;
                textObj["touid"] = _user_info->_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

                //发送并清空之前累计的消息列表
                text_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //通过TCP发送请求给chat server
                emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }

            //消息长度加本次循环的消息长度没超过1kb，就先将消息缓存在消息体中，并先行在界面展示
            //将文本气泡框与uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;
            text_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Msg = msgList[i].content.toUtf8();//转成urf8防止乱码
            obj["content"] = QString::fromUtf8(utf8Msg);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            auto text_msg = std::make_shared<TextChatData>(uuidString, obj["content"].toString(),
                                                           user_info->_uid, _user_info->_uid);
            emit sig_append_send_chat_msg(text_msg);//待发送消息缓存到本地队列中
        }
        //2.处理图片消息
        else if(type == "image")
        {
            pBubble = new PictureBubble(role, QPixmap(msgList[i].content));
        }
        //3.处理文件消息
        else if(type == "file")//文件还没做
        {

        }

        // 0.在界面展示消息
        if(pBubble != nullptr)
        {
            pChatItem->SetWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }

    //直到循环退出，消息长度依然不超过1kb（发送消息长度累计+消息列表中当前消息长度），就直接发送给服务器
    qDebug() << "textArray is " << textArray ;
    //发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->_uid;
    textObj["touid"] = _user_info->_uid;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    text_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}

//接收按钮测试代码（发布软件则隐藏此按钮）
void ChatPage::on_receive_btn_clicked()
{
    auto pTextEdit = ui->chat_edit;
    ChatRole role = ChatRole::Other;
    QString userName = QStringLiteral("Receiver");
    QString userIcon = ":/res/head_2.jpg";

    const QVector<MsgInfo> &msgList = pTextEdit->getMsgList();
    for(int i = 0; i < msgList.size(); ++i)
    {
        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->SetUserName(userName);
        pChatItem->SetUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        if(type == "text")//文本
        {
            pBubble = new TextBubble(role, msgList[i].content);
        }
        else if(type == "image")//图片
        {
            pBubble = new PictureBubble(role, QPixmap(msgList[i].content));
        }
        else if(type == "file")//文件还没做
        {

        }

        if(pBubble != nullptr)
        {
            pChatItem->SetWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }
}
