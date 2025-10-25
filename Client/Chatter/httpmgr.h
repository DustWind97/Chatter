#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include "global.h"

/******************************************************************************
 *
 * @file       httpmgr.h
 * @brief      Function: Http协议的相关报文处理
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
//去学C++的CRTP--奇异递归
class HttpMgr : public QObject, public Singleton<HttpMgr>/*以类本身为模板实例化*/,
                public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT//使用信号与槽所需要的宏

public:
    ~HttpMgr();
    //发送
    void PostHttpReq(QUrl url/*路由*/, QJsonObject json/*管理发送数据*/, ReqId req_id, Modules mod/*哪个模块的某个功能*/);

signals://信号在哪个类发的就定义在那儿
    void sig_http_finish(ReqId id, QString res/*结果*/, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId id, QString res/*结果*/, ErrorCodes err);//注册模块完成
    void sig_reset_mod_finish(ReqId id, QString res/*结果*/, ErrorCodes err);//重置密码模块完成
    void sig_login_mod_finish(ReqId id, QString res/*结果*/, ErrorCodes err);//登陆模块完成

private slots:
    void slot_http_finish(ReqId id, QString res/*结果*/, ErrorCodes err, Modules mod);

private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;
};


#endif // HTTPMGR_H
