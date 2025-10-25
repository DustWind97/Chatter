#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"    //非系统头文件尽量在cpp中包含，防止头文件互引用
#include "clickedlabel.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog),
    _countdown(5)
{
    ui->setupUi(this);

    //此界面是一个stackedWidget，分别有[注册]页以及[跳转回登陆]界面页
    ChangeReisgterPage();//进入此界面率先进入注册页

    //设置所有lineedit居中属性
    ui->user_edit->setAlignment(Qt::AlignCenter);
    ui->email_edit->setAlignment(Qt::AlignCenter);
    ui->pass_edit->setAlignment(Qt::AlignCenter);
    ui->confirm_edit->setAlignment(Qt::AlignCenter);
    ui->verify_edit->setAlignment(Qt::AlignCenter);

    //设置错误属性
    repolish(ui->reg_error_tip);//刷新状态
    ui->reg_error_tip->clear();//清除掉提示信息    初始设置为空字符

    //回包收到信号连接
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish,
            this, &RegisterDialog::slot_reg_mod_finish);

    //初始化http服务
    initHttpHandlers();



    /***********************************************************************************/
    //如下为注册界面输入合法性检测
    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });

    connect(ui->verify_edit, &QLineEdit::editingFinished, this, [this](){
        checkVerifyValid();
    });
    /***********************************************************************************/
    //设置浮动显示手形状
    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);

    ui->pass_visible->SetState("unvisible","unvisible_hover", "",
                               "visible",  "visible_hover",   "");

    ui->confirm_visible->SetState("unvisible","unvisible_hover","",
                                  "visible",  "visible_hover",  "");


    //设定初始为密码模式
    //ui->loginpass_visible->SetCurState(ClickLabelState::Selected);//初始设置为选中模式
    ui->pass_edit->setEchoMode(QLineEdit::Password);//密码模式
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    //连接点击事件
    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLabelState::Normal){
            ui->pass_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->pass_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLabelState::Normal){
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        }else{
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    /***********************************************************************************/
    // 创建定时器来计时
    _countdown_timer = new QTimer(this);
    // 连接信号和槽
    connect(_countdown_timer, &QTimer::timeout, [this](){
        if(_countdown == 0)
        {
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功, %1s 后返回登录").arg(_countdown);
        ui->tip_label->setText(str);
    });
}

RegisterDialog::~RegisterDialog()
{
    qDebug()<<"RegisterDialog was destructed";
    delete ui;
}

//获取验证码按钮
void RegisterDialog::on_get_code_btn_clicked()
{
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)"); //正则表达式
    bool match = regex.match(email).hasMatch();
    if(match){
        //发送http验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(gate_url_prefix + "/get_verifycode" /*测试用QUrl("http://localhost:8080/get_verifycode")*/,
                                            json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::REGISTERMOD);

    }else{
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
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

void RegisterDialog::showTip(QString str, bool b_ok)
{
    //正常状态置1，错误状态置0
    //设定标签的初始颜色
    if(b_ok){
        ui->reg_error_tip->setProperty("state","normal");
    }else{
        ui->reg_error_tip->setProperty("state","error");
    }

    ui->reg_error_tip->setText(str);
    repolish(ui->reg_error_tip);//刷新状态
}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包的逻辑 id返回的lamda表达式
    _handlers.insert(ReqId::ID_GET_VERIFY_CODE,[this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        //错误处理
        if(error != ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"),false);
            qDebug() << "错误码" << error;
            return;
        }

        //成功处理
        auto email = jsonObj["email"].toString();

        showTip(tr("验证码已发送至邮箱，请注意查收"),true);
        qDebug() << "email is" << email;
    });

    //注册用户的回包逻辑
    _handlers.insert(ReqId::ID_REG_USER,[this](const QJsonObject& jsonObj ){
        int error = jsonObj["error"].toInt();
        //错误处理
        if(error != ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误,用户已经存在"),false);
            qDebug() << "错误码" << error;
            return;
        }

        //成功处理
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"),true);
        qDebug() << "user uid is" << jsonObj["uid"].toString();
        qDebug() << "email is" << email;
        //跳转到注册界面的返回登录页
        ChangeTipPage();

    });

}

void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips,false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty())
    {
        ui->reg_error_tip->clear();
        return;
    }
    showTip(_tip_errs.first(),false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->pass_edit->text();
    auto confirm = ui->confirm_edit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regex("^[a-zA-Z0-9!@#$%^&*]{6,15}$");//添加正则匹配规则
    bool match = regex.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto pass = ui->pass_edit->text();
    auto confirm = ui->confirm_edit->text();

    if(confirm.length() < 6 || confirm.length() > 15 ){
        //提示长度不准确
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*.]{6,15}$");
    bool match = regExp.match(confirm).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_CONFIRM_ERR);

    if(pass != confirm){
        //提示密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码和密码不匹配"));
        return false;
    }else{
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }
    return true;
}

bool RegisterDialog::checkVerifyValid()
{
    auto pass = ui->verify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VERIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VERIFY_ERR);
    return true;
}

void RegisterDialog::on_confirm_btn_clicked()
{
    bool valid = checkUserValid();
    if(!valid){
        return;
    }
    valid = checkEmailValid();
    if(!valid){
        return;
    }
    valid = checkPassValid();
    if(!valid){
        return;
    }
    valid = checkVerifyValid();
    if(!valid){
        return;
    }

    //发送注册账号请求
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = md5Encrypt(ui->pass_edit->text());//密码，用md5进行加密
    json_obj["confirm"] = md5Encrypt(ui->confirm_edit->text());//确认密码，用md5进行加密
    json_obj["verifycode"] = ui->verify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"), json_obj,
                                        ReqId::ID_REG_USER, Modules::REGISTERMOD);

}

//跳转注册界面的注册页
void RegisterDialog::ChangeReisgterPage()
{
    ui->stackedWidget->setCurrentWidget(ui->register_page);
}

//跳转注册界面的提示页（提示返回登陆）
void RegisterDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->backtologin_page);

    // 启动定时器，设置间隔为1000毫秒（1秒）
    _countdown_timer->start(1000);
}

//在提示页返回登陆
void RegisterDialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

//在注册页返回登陆
void RegisterDialog::on_cancel_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

