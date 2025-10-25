#include "authenfriend.h"
#include "ui_authenfriend.h"
#include "clickedlabel.h"
#include "friendlabel.h"
#include <QScrollBar>
#include "usermgr.h"
#include "tcpmgr.h"

AuthenFriend::AuthenFriend(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuthenFriend)
    , _label_point(2,6)
{
    ui->setupUi(this);
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("AuthenFriend");
    this->setModal(true);
    ui->label_edit->setPlaceholderText("搜索、添加标签");
    ui->back_edit->setPlaceholderText("尘风");

    ui->label_edit->SetMaxLength(21);
    ui->label_edit->move(2, 2);
    ui->label_edit->setFixedHeight(20);
    ui->label_edit->setMaxLength(10);
    ui->input_tip_wid->hide();

    _tip_cur_point = QPoint(5, 5);

    _tip_data = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计",
                 "父与子学Python","nodejs开发指南","go 语言开发指南",
                 "游戏伙伴","金融投资","微信读书","拼多多拼友" };

    connect(ui->more_label, &ClickedOnceLabel::clicked, this, &AuthenFriend::ShowMoreLabel);
    InitTipLabels();
    //链接输入标签回车事件
    connect(ui->label_edit, &CustomizeEdit::returnPressed, this, &AuthenFriend::SlotLabelEnter);
    connect(ui->label_edit, &CustomizeEdit::textChanged, this, &AuthenFriend::SlotLabelTextChange);
    connect(ui->label_edit, &CustomizeEdit::editingFinished, this, &AuthenFriend::SlotLabelEditFinished);
    connect(ui->tip_label, &ClickedOnceLabel::clicked, this, &AuthenFriend::SlotAddFirendLabelByClickTip);

    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);
    ui->sure_btn->SetState("normal","hover","press");
    ui->cancel_btn->SetState("normal","hover","press");
    //连接确认和取消按钮的槽函数
    connect(ui->cancel_btn, &QPushButton::clicked, this, &AuthenFriend::SlotApplyCancel);
    connect(ui->sure_btn, &QPushButton::clicked, this, &AuthenFriend::SlotApplySure);
}

AuthenFriend::~AuthenFriend()
{
    qDebug()<< "AuthenFriend destruct";
    delete ui;
}

void AuthenFriend::InitTipLabels()
{
    int lines = 1;
    for(std::size_t i = 0; i < _tip_data.size(); i++){

        auto* label = new ClickedLabel(ui->label_list);
        label->SetState("normal", "hover", "pressed", "selected_normal",
                     "selected_hover", "selected_pressed");
        label->setObjectName("tipslb");
        label->setText(_tip_data[i]);
        connect(label, &ClickedLabel::clicked, this, &AuthenFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(label->font()); // 获取QLabel控件的字体信息
        // 获取文本的宽度
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                int textWidth = fontMetrics.width(label->text);
        #else
                int textWidth = fontMetrics.horizontalAdvance(label->text());
        #endif
        // 获取文本的高度
        int textHeight = fontMetrics.height();

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->label_list->width()) {
            lines++;
            if (lines > 2) {
                delete label;
                return;
            }

            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }

        auto next_point = _tip_cur_point;
        AddTipLabels(label, _tip_cur_point,next_point, textWidth, textHeight);
        _tip_cur_point = next_point;
    }
}

void AuthenFriend::AddTipLabels(ClickedLabel* label, QPoint cur_point, QPoint& next_point, int text_width, int text_height)
{
    Q_UNUSED(text_height);

    label->move(cur_point);
    label->show();
    _add_labels.insert(label->text(), label);
    _add_label_keys.push_back(label->text());
    next_point.setX(label->pos().x() + text_width + 15);
    next_point.setY(label->pos().y());
}

bool AuthenFriend::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->scrollArea && event->type() == QEvent::Enter)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if (obj == ui->scrollArea && event->type() == QEvent::Leave)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(obj, event);
}

void AuthenFriend::SetApplyInfo(std::shared_ptr<ApplyInfo> apply_info)
{
    _apply_info = apply_info;
    ui->back_edit->setPlaceholderText(apply_info->_name);
}

void AuthenFriend::ShowMoreLabel()
{
    qDebug()<< "receive more label clicked";
    ui->more_label_wid->hide();

    ui->label_list->setFixedWidth(325);
    _tip_cur_point = QPoint(5, 5);
    auto next_point = _tip_cur_point;
    int textWidth;
    int textHeight;
    //重拍现有的label
    for(auto & added_key : _add_label_keys){
        auto added_label = _add_labels[added_key];

        QFontMetrics fontMetrics(added_label->font()); // 获取QLabel控件的字体信息
        // 获取文本的宽度
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                textWidth = fontMetrics.width(added_label->text);
        #else
                textWidth = fontMetrics.horizontalAdvance(added_label->text());
        #endif
        // 获取文本的高度
        textHeight = fontMetrics.height();

        if(_tip_cur_point.x() +textWidth + tip_offset > ui->label_list->width()){
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y()+textHeight+15);
        }
        added_label->move(_tip_cur_point);

        next_point.setX(added_label->pos().x() + textWidth + 15);
        next_point.setY(_tip_cur_point.y());

        _tip_cur_point = next_point;
    }

    //添加未添加的
    for(std::size_t i = 0; i < _tip_data.size(); i++){
        auto iter = _add_labels.find(_tip_data[i]);
        if(iter != _add_labels.end()){
            continue;
        }

        auto* label = new ClickedLabel(ui->label_list);
        label->SetState("normal", "hover", "pressed", "selected_normal",
                     "selected_hover", "selected_pressed");
        label->setObjectName("tipslb");
        label->setText(_tip_data[i]);
        connect(label, &ClickedLabel::clicked, this, &AuthenFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(label->font()); // 获取QLabel控件的字体信息
        // 获取文本的宽度
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            int textWidth = fontMetrics.width(label->text);
        #else
            int textWidth = fontMetrics.horizontalAdvance(label->text());
        #endif
        // 获取文本的高度
        textHeight = fontMetrics.height();

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->label_list->width()) {

            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);

        }

        next_point = _tip_cur_point;

        AddTipLabels(label, _tip_cur_point, next_point, textWidth, textHeight);

        _tip_cur_point = next_point;

    }

    int diff_height = next_point.y() + textHeight + tip_offset - ui->label_list->height();
    ui->label_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    //qDebug()<<"after resize ui->lb_list size is " <<  ui->lb_list->size();
    ui->scrollContent->setFixedHeight(ui->scrollContent->height()+diff_height);
}

void AuthenFriend::resetLabels()
{
    auto max_width = ui->grid_wid->width();
    auto label_height = 0;
    for(auto iter = _friend_labels.begin(); iter != _friend_labels.end(); iter++){
        //todo... 添加宽度统计
        if( _label_point.x() + iter.value()->width() > max_width) {
            _label_point.setY(_label_point.y()+iter.value()->height()+6);
            _label_point.setX(2);
        }

        iter.value()->move(_label_point);
        iter.value()->show();

        _label_point.setX(_label_point.x()+iter.value()->width()+2);
        _label_point.setY(_label_point.y());
        label_height = iter.value()->height();
    }

    if(_friend_labels.isEmpty()){
        ui->label_edit->move(_label_point);
        return;
    }

    if(_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->grid_wid->width()){
        ui->label_edit->move(2,_label_point.y()+label_height+6);
    }else{
        ui->label_edit->move(_label_point);
    }
}

void AuthenFriend::addLabel(QString name)
{
    if (_friend_labels.find(name) != _friend_labels.end()) {
        return;
    }

    auto tmplabel = new FriendLabel(ui->grid_wid);
    tmplabel->SetText(name);
    tmplabel->setObjectName("FriendLabel");

    auto max_width = ui->grid_wid->width();
    //todo... 添加宽度统计
    if (_label_point.x() + tmplabel->width() > max_width) {
        _label_point.setY(_label_point.y() + tmplabel->height() + 6);
        _label_point.setX(2);
    }
    else {

    }


    tmplabel->move(_label_point);
    tmplabel->show();
    _friend_labels[tmplabel->Text()] = tmplabel;
    _friend_label_keys.push_back(tmplabel->Text());

    connect(tmplabel, &FriendLabel::sig_close, this, &AuthenFriend::SlotRemoveFriendLabel);

    _label_point.setX(_label_point.x() + tmplabel->width() + 2);

    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->grid_wid->width()) {
        ui->label_edit->move(2, _label_point.y() + tmplabel->height() + 2);
    }
    else {
        ui->label_edit->move(_label_point);
    }

    ui->label_edit->clear();

    if (ui->grid_wid->height() < _label_point.y() + tmplabel->height() + 2) {
        ui->grid_wid->setFixedHeight(_label_point.y() + tmplabel->height() * 2 + 2);
    }
}

void AuthenFriend::SlotLabelEnter()
{
    if(ui->label_edit->text().isEmpty()){
        return;
    }

    addLabel(ui->label_edit->text());

    ui->input_tip_wid->hide();
}

void AuthenFriend::SlotRemoveFriendLabel(QString name)
{
    qDebug() << "receive close signal";

    _label_point.setX(2);
    _label_point.setY(6);

    auto find_iter = _friend_labels.find(name);

    if(find_iter == _friend_labels.end()){
        return;
    }

    auto find_key = _friend_label_keys.end();
    for(auto iter = _friend_label_keys.begin(); iter != _friend_label_keys.end();
         iter++){
        if(*iter == name){
            find_key = iter;
            break;
        }
    }

    if(find_key != _friend_label_keys.end()){
        _friend_label_keys.erase(find_key);
    }


    delete find_iter.value();

    _friend_labels.erase(find_iter);

    resetLabels();

    auto find_add = _add_labels.find(name);
    if(find_add == _add_labels.end()){
        return;
    }

    find_add.value()->ResetNormalState();
}

//点击标已有签添加或删除新联系人的标签
void AuthenFriend::SlotChangeFriendLabelByTip(QString lbtext, ClickLabelState state)
{
    auto find_iter = _add_labels.find(lbtext);
    if(find_iter == _add_labels.end()){
        return;
    }

    if(state == ClickLabelState::Selected){
        //编写添加逻辑
        addLabel(lbtext);
        return;
    }

    if(state == ClickLabelState::Normal){
        //编写删除逻辑
        SlotRemoveFriendLabel(lbtext);
        return;
    }
}

void AuthenFriend::SlotLabelTextChange(const QString& text)
{
    if (text.isEmpty()) {
        ui->tip_label->setText("");
        ui->input_tip_wid->hide();
        return;
    }

    auto iter = std::find(_tip_data.begin(), _tip_data.end(), text);
    if (iter == _tip_data.end()) {
        auto new_text = add_prefix + text;
        ui->tip_label->setText(new_text);
        ui->input_tip_wid->show();
        return;
    }
    ui->tip_label->setText(text);
    ui->input_tip_wid->show();
}

void AuthenFriend::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void AuthenFriend::SlotAddFirendLabelByClickTip(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1) {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);
    //标签展示栏也增加一个标签, 并设置绿色选中
    if (index != -1) {
        _tip_data.push_back(text);
    }

    auto* label = new ClickedLabel(ui->label_list);
    label->SetState("normal", "hover", "pressed", "selected_normal",
                    "selected_hover", "selected_pressed");
    label->setObjectName("tipslb");
    label->setText(text);
    connect(label, &ClickedLabel::clicked, this, &AuthenFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->label_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();

    QFontMetrics fontMetrics(label->font()); // 获取QLabel控件的字体信息
    // 获取文本的宽度
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        //int textWidth = fontMetrics.width(label->text);
    #else
        int textWidth = fontMetrics.horizontalAdvance(label->text());
    #endif
    // 获取文本的高度
    int textHeight = fontMetrics.height();

    if (_tip_cur_point.x() + textWidth+ tip_offset+3 > ui->label_list->width()) {

        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);

    }

    auto next_point = _tip_cur_point;

    AddTipLabels(label, _tip_cur_point, next_point, textWidth,textHeight);
    _tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->label_list->height();
    ui->label_list->setFixedHeight(next_point.y() + textHeight + tip_offset);

    label->SetCurState(ClickLabelState::Selected);

    ui->scrollContent->setFixedHeight(ui->scrollContent->height()+ diff_height );
}

//同意认证处理
void AuthenFriend::SlotApplySure()
{
    qDebug() << "Slot Apply Sure ";
    //添加发送逻辑
    QJsonObject jsonObj;
    auto uid = UserMgr::GetInstance()->GetUid();//获取本人uid
    jsonObj["fromuid"] = uid;
    jsonObj["touid"] = _apply_info->_uid;
    QString back_name = "";
    if(ui->back_edit->text().isEmpty()){
        back_name = ui->back_edit->placeholderText();
    }else{
        back_name = ui->back_edit->text();
    }
    jsonObj["back"] = back_name;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_AUTH_FRIEND_REQ, jsonData);

    this->hide();
    deleteLater();
}

void AuthenFriend::SlotApplyCancel()
{
    this->hide();
    deleteLater();
}
