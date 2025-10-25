#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpmgr.h"
#include "clickedlabel.h"
#include "tcpmgr.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    //初始属性设置---lineedit居中
    ui->email_edit->setAlignment(Qt::AlignCenter);
    ui->pass_edit->setAlignment(Qt::AlignCenter);
    ui->email_edit->setPlaceholderText("邮箱/名字");
    ui->pass_edit->setPlaceholderText("请输入密码");

    //设置错误属性
    repolish(ui->login_error_tip);//刷新状态
    ui->login_error_tip->clear();//清除掉提示信息    初始设置为空字符

    //设置浮动显示手形状
    ui->loginpass_visible->setCursor(Qt::PointingHandCursor);
    ui->loginpass_visible->SetState("unvisible","unvisible_hover", "",
                                    "visible",  "visible_hover",   "");//设定文本输入框初始为密码模式
    ui->pass_edit->setEchoMode(QLineEdit::Password);//密码模式

    //连接点击事件
    connect(ui->loginpass_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->loginpass_visible->GetCurState();
        if(state == ClickLabelState::Normal){
         ui->pass_edit->setEchoMode(QLineEdit::Password);
        }else{
         ui->pass_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
        });


    //连接切换注册界面消息
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::sigSwitchRegister);

    //连接切换重置密码界面消息
    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    ui->forget_label->setCursor(Qt::PointingHandCursor);
    //连接切换重置密码界面
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::sigSwitchReset);

    initHttpHandlers();
    //连接登录回包信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this,
            &LoginDialog::slot_login_mod_finish);


    //连接tcp连接请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect); //这句最好是写到tcpmgr中
    //连接tcp管理者发出的连接成功信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_con_success, this, &LoginDialog::slot_tcp_con_finish);
    //连接tcp管理者发出的登陆失败信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_login_failed, this, &LoginDialog::slot_login_failed);
}

LoginDialog::~LoginDialog()
{
    qDebug()<<"LoginDialog was destructed";
    delete ui;
}

void LoginDialog::keyPressEvent(QKeyEvent *event)
{
    //设置登录按钮快捷键
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        ui->login_btn->click(); // 触发按钮点击
        event->accept();        // 标记事件已处理
    }

    QDialog::keyPressEvent(event); // 其他按键正常处理
}

void LoginDialog::showTip(QString str, bool b_ok)
{
    //正常状态置1，错误状态置0
    //设定标签的初始颜色
    if(b_ok){
        ui->login_error_tip->setProperty("state","normal");
    }else{
        ui->login_error_tip->setProperty("state","error");
    }

    ui->login_error_tip->setText(str);
    repolish(ui->login_error_tip);//刷新状态
}

void LoginDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void LoginDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->login_error_tip->clear();
        return;
    }

    showTip(_tip_errs.first(), false);
}

void LoginDialog::initHttpHandlers()
{
    //注册获取登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            qDebug() << "错误码" << error;
            enableBtn(true);
            return;
        }

        auto email = jsonObj["email"].toString();
        auto user = jsonObj["user"].toString();

        //发送信号通知tcpMgr发送长链接
        ServerInfo server_info;
        server_info.Uid = jsonObj["uid"].toInt();//uid用来标识登陆账户的账号（邮箱或者用户名都可）
        server_info.Host = jsonObj["host"].toString();
        server_info.Port = jsonObj["port"].toString();
        server_info.Token = jsonObj["token"].toString();//token用来标识登陆账户的密码验证

        _uid = server_info.Uid;
        _token = server_info.Token;
        qDebug() << "网关服务器（Http连接）回复信息: "<< "user is " << user << ", email is " << email << ", uid is " << server_info.Uid <<", host is "
                 << server_info.Host << ", Port is " << server_info.Port << ", Token is " << server_info.Token;
        emit sig_connect_tcp(server_info);//申请建立TCP长连接
    });
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    //不成功
    if(err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), false);
        return;
    }

    //解析JSON字符串,将 res 转换为 QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8()); //xxx.json文件了
    //Json解析失败
    if(jsonDoc.isNull())//文件为空，解析失败
    {
        showTip(tr("Json解析失败"),false);
        return;
    }

    //Json解析错误
    if(!jsonDoc.isObject())
    {
        showTip(tr("Json解析失败"),false); //提示是给到用户的，错误类型写入到日志文件中
        return;
    }

    //丢给处理器具体的函数处理相应内容
    _handlers[id](jsonDoc.object());
    return;

}

//验证用户名输入合法性
bool LoginDialog::checkUserValid()
{
    auto user = ui->email_edit->text();
    if(user.isEmpty()){
        qDebug() << "user empty " ;
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));//后面改为邮箱登陆
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

//验证密码输入合法性
bool LoginDialog::checkPwdValid()
{
    auto pwd = ui->pass_edit->text();
    if(pwd.length() < 6 || pwd.length() > 15){
        qDebug() << "Pass length invalid";
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(pwd).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符且长度为(6~15)"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

//点击登录按钮
void LoginDialog::on_login_btn_clicked()
{
    qDebug()<<"Login Button was clicked";
    if(checkUserValid() == false){
        return;
    }
    if(checkPwdValid() == false){
        return ;
    }

    enableBtn(false);
    auto email = ui->email_edit->text();
    auto pass = ui->pass_edit->text();

    //发送http请求登录
    QJsonObject json_obj;
    json_obj["email"] = email;//用户名登录还是邮箱登陆
    json_obj["passwd"] = md5Encrypt(pass);//md5加密 密码（后续都需要修改）
    //json_obj["passwd"] = xorString(pass);//异或加密 密码
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                        json_obj, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
}

//设置按钮是否可用
bool LoginDialog::enableBtn(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
    ui->reg_btn->setEnabled(enabled);
    return true;
}

//TCP连接完成
void LoginDialog::slot_tcp_con_finish(bool b_success)
{
    if(b_success){
        showTip(tr("聊天服务连接成功，正在登录..."), true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        QJsonDocument doc(jsonObj);
        QByteArray jsonByte = doc.toJson(QJsonDocument::Indented);

        //发送tcp请求给chatserver 建立TCP连接
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonByte);//最好是添加emit

        //qDebug() << "发送数据成功";

    }else{
        showTip(tr("网络异常"),false);
        enableBtn(true);
    }
}

void LoginDialog::slot_login_failed(int err)
{
    QString result = QString("登录失败, err is %1").arg(err);
    showTip(result,false);
    enableBtn(true);
}

