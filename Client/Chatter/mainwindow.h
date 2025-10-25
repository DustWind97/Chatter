#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      Function: 主界面
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/

class LoginDialog;    // 登录
class RegisterDialog; // 注册
class ResetDialog;    // 重置
class ChatDialog;     // 聊天

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void SlotSwitchReg();   // 切换到注册界面
    void SlotSwitchReset(); // 切换到重置密码界面
    void SlotSwitchLogin(); // 切换到登录界面
    void SlotSwitchChat();  // 切换到聊天界面

private:
    Ui::MainWindow *ui;
    QStackedWidget *_stacked_widget; // 界面堆栈容器

    // 界面指针 - 使用懒加载方式创建
    LoginDialog    *_login_dlg = nullptr; // 登录页
    RegisterDialog *_reg_dlg   = nullptr; // 注册页
    ResetDialog    *_reset_dlg = nullptr; // 重置页
    ChatDialog     *_chat_dlg  = nullptr; // 聊天页
};
#endif // MAINWINDOW_H
