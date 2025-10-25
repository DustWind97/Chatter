#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

/******************************************************************************
 *
 * @file       resetdialog.h
 * @brief      Function: 重置密码界面
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private:
    bool checkUserValid();                  // 检查用户名合法性
    bool checkPassValid();                  // 检查密码合法性
    bool checkEmailValid();                 // 检查邮箱合法性
    bool checkVerifyValid();                // 检查验证码合法性

    void showTip(QString str,bool b_ok);    // 是否显示提示
    void AddTipErr(TipErr te,QString tips); // 添加提示信息
    void DelTipErr(TipErr te);              // 删除提示信息
    void initHttpHandlers();                // HTTP 报文回调处理

signals:
    void sigSwitchLogin();

private slots:
    void slot_reset_mod_finish(ReqId id, QString res/*结果*/, ErrorCodes err);
    void on_return_btn_clicked();
    void on_verify_btn_clicked();
    void on_confirm_btn_clicked();

private:
    Ui::ResetDialog *ui;

    QMap<ReqId/*请求id*/,std::function<void(const QJsonObject&)>>/*函数对象*/ _handlers;//函数处理用map存储函数指针
    QMap<TipErr, QString> _tip_errs;
};

#endif // RESETDIALOG_H
