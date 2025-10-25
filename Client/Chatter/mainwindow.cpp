#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Chatter聊天室");

    // 创建堆栈容器并设为中央部件
    _stacked_widget = new QStackedWidget(this);
    setCentralWidget(_stacked_widget);// 通过堆栈界面来管理各个界面

    // 初始化登录页并添加到堆栈
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint); // 设置自定义无边框
    _stacked_widget->addWidget(_login_dlg);

    // 连接登录界面信号
    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchReg); // 切换到注册界面
    connect(_login_dlg, &LoginDialog::sigSwitchReset, this, &MainWindow::SlotSwitchReset);  // 切换到重置密码界面

    // 连接TCP管理器信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_swich_chatdlg, this, &MainWindow::SlotSwitchChat);

    // 测试用（可注释掉）
    //emit TcpMgr::GetInstance()->sig_swich_chatdlg();
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow was destructed";
    delete ui;
    // 不需要手动删除子部件，Qt对象树会自动管理
}

// 切换到注册界面
void MainWindow::SlotSwitchReg()
{
    // 懒加载:首次使用时创建
    if (!_reg_dlg)
    {
        _reg_dlg = new RegisterDialog(this);
        _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);//设置自定义无边框
        _stacked_widget->addWidget(_reg_dlg);//将此页面添加到页面栈中

        // 连接注册页返回登录页信号
        connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    }

    _stacked_widget->setCurrentWidget(_reg_dlg);//将注册页设置为页面栈的显示界面
}

// 切换到重置密码界面
void MainWindow::SlotSwitchReset()
{
    // 懒加载:首次使用时创建
    if (!_reset_dlg)
    {
        _reset_dlg = new ResetDialog(this);
        _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);//设置自定义无边框
        _stacked_widget->addWidget(_reset_dlg);//将此页面添加到页面栈中

        // 连接从重置页返回登录页信号
        connect(_reset_dlg, &ResetDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    }

    _stacked_widget->setCurrentWidget(_reset_dlg);//将重置页设置为页面栈的显示界面
}

// 切换到聊天界面
void MainWindow::SlotSwitchChat()
{
    // 懒加载：首次使用时创建，登录到聊天主界面优先显示聊天界面（联系人界面需要手动去点击切换）
    if (!_chat_dlg) {
        _chat_dlg = new ChatDialog(this);
        _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);//设置自定义无边框
        _stacked_widget->addWidget(_chat_dlg);//将聊天页添加到页面栈中

        // 调整窗口大小
        this->setMinimumSize(QSize(1050, 900));
        this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

        // 设置窗口居中
        QMetaObject::invokeMethod(this, [this] {
            QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
            QSize windowSize = this->size();
            QPoint centerPos = QPoint((screenRect.width() - windowSize.width()) / 2,
                                      (screenRect.height() - windowSize.height()) / 2);
            this->move(centerPos);
        }, Qt::QueuedConnection);
    }

    _stacked_widget->setCurrentWidget(_chat_dlg);//将聊天页设置为页面栈的显示界面
}


// 切换回登录界面（通用）
void MainWindow::SlotSwitchLogin()
{
    Q_ASSERT(_login_dlg != nullptr);// 确保登录界面已存在
    _stacked_widget->setCurrentWidget(_login_dlg);//将登录页设置为页面栈的显示界面
}
