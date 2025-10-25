#ifndef TCPMGR_H
#define TCPMGR_H
#include <QTcpSocket>
#include "singleton.h"
#include "global.h"
#include <functional>
#include <QObject>
#include "userdata.h"
#include <QJsonArray>

//以前的阻塞模式：仅适合单服务器单连接情况
//非阻塞模式：当今linux环境下常用epoll，Windows环境下常用iocp，asio封装了前面两个模型
//同步异步，阻塞非阻塞的区别

/******************************************************************************
 *
 * @file       tcpmgr.h
 * @brief      Function: TCP协议相关的处理，长连接
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT

public:
    ~TcpMgr();

private:
    friend class Singleton<TcpMgr>;
    TcpMgr();
    void initTcpHandlers();//处理回调
    void handleMsg(ReqId id, int len, QByteArray data);//处理信息

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QByteArray dataBytes);
    void sig_swich_chatdlg();//切换聊天窗口
    void sig_load_apply_list(QJsonArray json_array);
    void sig_login_failed(int err);
    void sig_user_search(std::shared_ptr<SearchInfo> search_info);//用户搜索
    void sig_friend_apply(std::shared_ptr<AddFriendApply> apply_info);//对方的好友申请
    void sig_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);//我发的好友申请，对方同意好友申请
    void sig_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);//别人发的还有申请，我同意申请好友的回复
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);

public slots:
    void slot_tcp_connect(ServerInfo si);
    void slot_send_data(ReqId reqId, QByteArray dataBytes);         //发送TCP数据给ChatServer
    //void sig_login_failed(int err);

private:
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;//缓存
    bool _b_recv_pending;//接收完整性标志  pending（悬而未决的）
    quint16 _message_id;//两字节16bit unsigned short
    quint16 _message_len;//两字节16bit unsigned short
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray dataBytes)>> _handlers;

};

#endif // TCPMGR_H
