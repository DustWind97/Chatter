#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
#include <QTimer>

/******************************************************************************
 *
 * @file       registerdialog.h
 * @brief      Function: 注册界面
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
    void sigSwitchLogin();

private slots:
    void slot_reg_mod_finish(ReqId id, QString res/*结果*/, ErrorCodes err);
    void on_get_code_btn_clicked();         // 获取验证码按钮
    void on_confirm_btn_clicked();          // 注册页的注册确认按钮
    void on_return_btn_clicked();           // 注册页的返回登陆页按钮
    void on_cancel_btn_clicked();           // 提示页的返回登陆页按钮

private:
    void ChangeReisgterPage();              // 切换注册页
    void ChangeTipPage();                   // 切换提示页
    void showTip(QString str, bool b_ok);   // 提示处理
    void initHttpHandlers();                // 初始化http的处理

    //合法性检验最好是写成一个鼠标事件来检测
    void AddTipErr(TipErr te,QString tips); // 添加错误信息
    void DelTipErr(TipErr te);              // 删除错误信息
    bool checkUserValid();                  // 用户名合法性检验
    bool checkEmailValid();                 // 邮箱合法性检验
    bool checkPassValid();                  // 密码合法性检验
    bool checkConfirmValid();               // 确认密码合法性检验
    bool checkVerifyValid();                // 验证码合法性检验

private:
    Ui::RegisterDialog *ui;
    QMap<ReqId/*请求id*/,std::function<void(const QJsonObject&)>>/*函数对象*/ _handlers;//函数处理用map存储函数指针
    QMap<TipErr, QString> _tip_errs;        // 保存错误信息
    QTimer *_countdown_timer;               // 用于跳转计时
    int _countdown;                         // 计时器 秒钟数
};

#endif // REGISTERDIALOG_H
