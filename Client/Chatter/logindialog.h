#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include "global.h"

/******************************************************************************
 *
 * @file       logindialog.h
 * @brief      Function: 登录界面
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

protected:
    void keyPressEvent(QKeyEvent *event) override;//重写按键按下事件

private:
    bool checkUserValid();
    bool checkPwdValid();
    void showTip(QString str, bool b_ok);
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    void initHttpHandlers();  //初始化http的处理
    bool enableBtn(bool enabled);//网络错误时失效按钮

signals:
    void sigSwitchRegister();//此信号函数只是用于触发从 logindialog 页面切换到 registerdialog 页面的过渡。无实际意义，因此没有函数实现
    void sigSwitchReset();//此信号函数只是用于触发从 logindialog 页面切换到 resetdialog 页面的过渡。无实际意义，因此没有函数实现
    void sig_connect_tcp(ServerInfo &info);//建立TCP长连接信号

private slots:
    void on_login_btn_clicked();
    void slot_login_mod_finish(ReqId id, QString res/*结果*/, ErrorCodes err);
    void slot_tcp_con_finish(bool b_success);
    void slot_login_failed(int err);

private:
    Ui::LoginDialog *ui;

    QMap<ReqId/*请求id*/,std::function<void(const QJsonObject&)>>/*函数对象*/ _handlers;//函数处理用map存储函数指针
    QMap<TipErr, QString> _tip_errs; //错误提示
    int _uid;                        //缓存uid
    QString _token;                  //缓存token
};

#endif // LOGINDIALOG_H
