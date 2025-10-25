#include "httpmgr.h"

HttpMgr::HttpMgr()
{
    //连接信号和槽
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

HttpMgr::~HttpMgr()
{

}

//发送HTTP请求
void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod) //id和mod是用作回调使用
{
    QByteArray data = QJsonDocument(json).toJson();//序列化成字符串
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");//请求包类型
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));//请求包长度

    //共享自身生成智能指针（防止收到包时http实例已经被杀死）
    auto self = shared_from_this();

    QNetworkReply * reply = _manager.post(request, data); //此指针发送完成需要回收
    QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod](){
        //处理错误情况
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            //发送信号通知其他界面完成
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            //回收reply
            reply->deleteLater();
            return;
        }

        //无错误
        QString res = reply->readAll();
        //发送信号通知其他界面完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        //回收reply
        reply->deleteLater();
        return;//此return可写可不写
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD)
    {
        //发送信号通知指定模块http的响应结束
        emit sig_reg_mod_finish(id , res, err);//注册模块完成信号
    }

    if(mod == Modules::RESETMOD)
    {
        emit sig_reset_mod_finish(id , res, err);//重置密码模块完成信号
    }

    if(mod == Modules::LOGINMOD)
    {
        emit sig_login_mod_finish(id, res, err);//登录模块完成信号
    }
}
