#include "tcpmgr.h"
#include "usermgr.h"

TcpMgr::TcpMgr()
    :_host("")
    ,_port(0)
    ,_b_recv_pending(false)
    ,_message_id(0)
    ,_message_len(0)
{
    // 连接
    QObject::connect(&_socket, &QTcpSocket::connected, [&]() {
        qDebug() << "已建立TCP连接";
        // 连接建立后发送消息
        emit sig_con_success(true);
    });

    // 读取数据
    QObject::connect(&_socket, &QTcpSocket::readyRead, [&]() {
        // 当有数据可读时，读取所有数据
        // 读取所有数据并追加到缓冲区
        _buffer.append(_socket.readAll());

        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);//Qt6也差不多

        // 表示无限循环（Qt定义的宏）---为什么不能用while循环
        forever {
            // buffer = 2字节id + 2字节长度 + data数据域
            // 先解析头部
            if(!_b_recv_pending){
                // 检查缓冲区中的数据是否足够解析出一个消息头（消息ID + 消息长度）
                if (_buffer.size() < static_cast<int>(sizeof(quint16) * 2)) {
                    return; // 数据不够，等待更多数据
                }

                // 预读取消息ID和消息长度，但不从缓冲区中移除
                stream >> _message_id >> _message_len;

                // 将buffer 中的前四个字节移除
                _buffer = _buffer.mid(sizeof(quint16) * 2);

                // 输出读取的数据
                qDebug() << "Message ID:" << _message_id << ", Length:" << _message_len;
            }

            //buffer剩余长读是否满足消息体长度，不满足则退出继续等待接受
            if(_buffer.size() < _message_len){
                _b_recv_pending = true;//置为true，下一次进入循环就会自动跳过头部解析
                return;
            }

            _b_recv_pending = false;//收全了就置为false为下一次数据接收重置flag
            // 读取消息体
            QByteArray messageBody = _buffer.mid(0, _message_len);
            //qDebug() << "receive body msg is " << messageBody ;

            _buffer = _buffer.mid(_message_len);//完成一次数据获取，将这次长度以后得消息截取放在buffer中进行下一次数据截取
            handleMsg(ReqId(_message_id), _message_len, messageBody);
        }

    });

    // 处理错误（适用于 Qt5.15 之后版本）
    QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                    [&](QAbstractSocket::SocketError socketError) {
                            //Q_UNUSED(socketError)
                            qDebug() << "Socket Error Code: " << socketError
                                     << ", Error String: " << _socket.errorString();
                        });

    // 处理连接断开
    QObject::connect(&_socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Disconnected from server.";
    });

    // 发送数据信号，具体在槽函数进行
    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);

    //注册消息
    initTcpHandlers();
}

// 处理TCP报文回调
void TcpMgr::initTcpHandlers()
{
    //此处不能用，因为构造函数都还没完成，所以无法调用自己
    //auto self = shared_from_this();

    //1.用户登陆回复（本端发送用户登录请求，服务器回复，本端进行处理）
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug()<< "handle id is "<< id ;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);// 将QByteArray转换为QJsonDocument
        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "ID_CHAT_LOGIN_RSP failed, Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error"))
        {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_CHAT_LOGIN_RSP failed, err is Json Parse Err" << err ;
            emit sig_login_failed(err);
            return;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS)
        {
            qDebug() << "ID_CHAT_LOGIN_RSP failed, err is " << err ;
            emit sig_login_failed(err);
            return;
        }

        //成功处理逻辑
        qDebug() << "ID_CHAT_LOGIN_RSP success";
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto desc = jsonObj["desc"].toString();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex,"",desc);

        UserMgr::GetInstance()->SetUserInfo(user_info);
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());

        //有apply_list字段，则有人向他做申请
        if(jsonObj.contains("apply_list"))
        {
            UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
            //qDebug() << "ID_CHAT_LOGIN_RSP: apply list is " << jsonObj["apply_list"];
        }

        //有friend_list字段，好友列表
        if(jsonObj.contains("friend_list"))
        {
            UserMgr::GetInstance()->AppendFriendList(jsonObj["friend_list"].toArray());
            //qDebug() << "ID_CHAT_LOGIN_RSP: friend list is " << jsonObj["friend_list"];
        }

        emit sig_swich_chatdlg();
    });

    //2.好友搜索回复（本端发送好友搜索请求，服务器回复，本端进行处理）
    _handlers.insert(ReqId::ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug() << "handle id is " << id << " data is " << data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);// 将QByteArray转换为QJsonDocument
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "ID_SEARCH_USER_RSP Failed, Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error"))
        {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_SEARCH_USER_RSP Failed, err is Json Parse Err" << err;

            emit sig_user_search(nullptr);
            return;
        }
        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "ID_SEARCH_USER_RSP Failed, err is " << err;
            emit sig_user_search(nullptr);
            return;
        }

        //成功处理逻辑
        qDebug() << "ID_SEARCH_USER_RSP success";
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto desc = jsonObj["desc"].toString();
        auto search_info = std::make_shared<SearchInfo>(uid, name, nick, desc, sex,icon);

        emit sig_user_search(search_info);
    });

    //3.添加好友回复（本端发送添加好友请求，服务器回复，本端进行处理）
    _handlers.insert(ReqId::ID_ADD_FRIEND_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug() << "handle id is " << id << " data is " << data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);// 将QByteArray转换为QJsonDocument
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "ID_ADD_FRIEND_RSP failed, Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error"))
        {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_ADD_FRIEND_RSP failed, err is Json Parse Err" << err;

            //emit sig_friend_apply(nullptr);
            return;
        }
        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "ID_ADD_FRIEND_RSP failed, err is " << err;
            //emit sig_friend_apply(nullptr);
            return;
        }

        //成功处理逻辑
        qDebug() << "ID_ADD_FRIEND_RSP success";
    });

    //4.添加好友请求（对端发送添加好友请求，本端进行处理）
    _handlers.insert(ReqId::ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug() << "handle id is " << id << " data is " << data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);// 将QByteArray转换为QJsonDocument
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ failed, Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error"))
        {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ failed, err is Json Parse Err" << err;

            //emit sig_friend_apply(nullptr);
            return;
        }
        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ, err is " << err;
            //emit sig_friend_apply(nullptr);
            return;
        }

        //成功处理逻辑
        qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ success";
        //解析对方信息
        auto from_uid = jsonObj["applyuid"].toInt();  // 对方uid
        auto name = jsonObj["name"].toString();       // 对方名字
        auto desc = jsonObj["desc"].toString();       // 对方描述
        auto icon = jsonObj["icon"].toString();       // 对方头像
        auto nick = jsonObj["nick"].toString();       // 对方昵称
        auto sex = jsonObj["sex"].toInt();            // 对方性别

        //封装对方信息
        std::shared_ptr<AddFriendApply> apply_info = std::make_shared<AddFriendApply>(from_uid, name, desc, icon, nick, sex);
        qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ: fromuid is " << apply_info->_from_uid
                 << " name is " << apply_info->_name << " icon is " << apply_info->_icon << " desc is " << desc;
        emit sig_friend_apply(apply_info);//发送给处理界面
    });

    //5.好友认证回复（本端发送好友认证请求，服务器回复，本端进行处理）
    _handlers.insert(ReqId::ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug() << "handle id is " << id << " data is " << data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);// 将QByteArray转换为QJsonDocument
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "ID_AUTH_FRIEND_RSP failed, Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error"))
        {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_AUTH_FRIEND_RSP failed, err is Json Parse Err" << err;
            return;
        }
        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "ID_AUTH_FRIEND_RSP, err is " << err;
            return;
        }

        //成功处理逻辑
        qDebug() << "ID_AUTH_FRIEND_RSP success";
        //解析对方信息
        auto uid = jsonObj["uid"].toInt();//uid
        auto name = jsonObj["name"].toString();//对方名字
        auto nick = jsonObj["nick"].toString();//对方昵称
        auto icon = jsonObj["icon"].toString();//对方头像
        auto sex = jsonObj["sex"].toInt();//对方性别

        //封装对方信息
        std::shared_ptr<AuthRsp> auth_rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        qDebug() << "ID_AUTH_FRIEND_RSP: uid is " << auth_rsp->_uid
                 << " name is " << auth_rsp->_name << " icon is " << auth_rsp->_icon;

        emit sig_auth_rsp(auth_rsp);//发送给处理界面 渲染界面
    });

    //6.好友认证请求（对端发送好友认证请求，本端进行处理）
    _handlers.insert(ReqId::ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug() << "handle id is " << id << " data is " << data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);// 将QByteArray转换为QJsonDocument
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ failed, Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error"))
        {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ failed, err is Json Parse Err" << err;
            //emit sig_add_auth_friend(nullptr);
            return;
        }
        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ, err is " << err;
            //emit sig_add_auth_friend(nullptr);
            return;
        }

        //成功处理逻辑
        qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ success";
        //解析对方信息
        auto from_uid = jsonObj["fromuid"].toInt();//对方uid
        auto name = jsonObj["name"].toString();//对方名字
        auto nick = jsonObj["nick"].toString();//对方昵称
        auto icon = jsonObj["icon"].toString();//对方头像
        auto sex = jsonObj["sex"].toInt();//对方性别

        //封装对方信息
        std::shared_ptr<AuthInfo> auth_info = std::make_shared<AuthInfo>(from_uid, name, nick, icon, sex);
        qDebug() << "ID_NOTIFY_AUTH_FRIEND_REQ: uid is " << auth_info->_uid
                 << " name is " << auth_info->_name << " icon is " << auth_info->_icon;

        emit sig_add_auth_friend(auth_info);//发送给处理界面
    });

    //7.文本聊天回复（本端发送文本聊天请求，服务器回复，本端进行处理）
    _handlers.insert(ReqId::ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "ID_TEXT_CHAT_MSG_RSP failed, Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_TEXT_CHAT_MSG_RSP failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "ID_TEXT_CHAT_MSG_RSP Failed, err is " << err;

            return;
        }

        //成功处理逻辑
        qDebug() << "ID_TEXT_CHAT_MSG_RSP success";
        //ui设置送达等标记 todo...  已送达，已读
    });

    //8.文本聊天请求（对端发送文本聊天请求[携带文本消息]，本端进行处理）
    _handlers.insert(ReqId::ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(this);
        Q_UNUSED(len);

        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "ID_NOTIFY_TEXT_CHAT_MSG_REQ failed, Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_NOTIFY_TEXT_CHAT_MSG_REQ failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "ID_NOTIFY_TEXT_CHAT_MSG_REQ failed, err is " << err;
            return;
        }

        //成功处理逻辑
        qDebug() << "ID_NOTIFY_TEXT_CHAT_MSG_REQ success";
        auto chat_msg = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),
                                                     jsonObj["text_array"].toArray());
        qDebug() << "ID_NOTIFY_TEXT_CHAT_MSG_REQ: msgtext is " << jsonObj["text_array"];

        emit sig_text_chat_msg(chat_msg);
    });

}

//根据id去查找对应的回调函数
void TcpMgr::handleMsg(ReqId id, int len, QByteArray dataBytes)
{

    auto find_iter =  _handlers.find(id);//从handlers中选取对应的id回调执行
    if(find_iter == _handlers.end()){
        qDebug()<< "Not found id ["<< id << "] to handle";
        return ;
    }

    find_iter.value()(id, len, dataBytes);
}

//析构
TcpMgr::~TcpMgr()
{

}

//TCP连接槽函数（从GateServer获取待连接ChatServer的服务器信息[IP:Port]）
void TcpMgr::slot_tcp_connect(ServerInfo server_info)
{
    //尝试连接到服务器
    _host = server_info.Host;//IP
    _port = static_cast<uint16_t>(server_info.Port.toUInt());//端口

    qDebug() << "待连接TCP服务器信息 IP:" << _host << ", Port:" << _port;
    qDebug() << "正在建立TCP连接...";

    _socket.connectToHost(_host, _port);
}

//发送数据槽函数
void TcpMgr::slot_send_data(ReqId reqId, QByteArray dataBytes)
{
    uint16_t id = reqId;

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.length());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // 设置数据流使用网络字节序
    /*
     * 小端字节序：高字节在高位，低字节在低位---一致的为小端
     * 大端字节序：高字节在低位，低字节在高位
     */
    out.setByteOrder(QDataStream::BigEndian);

    //自己定义的数据包格式 ，这种分段的方式有利于处理粘包
    // 写入ID和长度
    out << id << len;

    // 添加字符串数据
    block.append(dataBytes);

    // 发送数据
    _socket.write(block);

    //qDebug() << "tcp mgr send byte data is " << block ;
}
