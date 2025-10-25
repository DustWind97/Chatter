#include "applyfriend.h"
#include "ui_applyfriend.h"
#include "clickedlabel.h"
#include "friendlabel.h"
#include "userdata.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include <QScrollBar>

ApplyFriend::ApplyFriend(QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::ApplyFriend)
    , _label_point(2,6)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("ApplyFirend");
    this->setModal(true);
    ui->name_edit->setPlaceholderText("DustWind");
    ui->label_edit->setPlaceholderText("搜索添加标签");//标签
    ui->back_edit->setPlaceholderText("丿尘风丨丶");//备注

    ui->label_edit->SetMaxLength(21);
    ui->label_edit->move(2,2);
    ui->label_edit->setFixedHeight(20);
    ui->input_tip_wid->hide();//先隐藏输入提示界面

    _tip_cur_point = QPoint(5, 5);

    //这些是应当从服务器获取的，这里做测试的
    _tip_data = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计",
                 "父与子学Python","nodejs开发指南","go 语言开发指南",
                 "游戏伙伴","金融投资","微信读书","拼多多拼友" };

    InitTipLabels();

    //链接更多信息展开标签
    connect(ui->more_label, &ClickedOnceLabel::clicked, this, &ApplyFriend::ShowMoreLabel);
    //链接输入标签回车事件
    connect(ui->label_edit, &CustomizeEdit::returnPressed, this, &ApplyFriend::SlotLabelEnter);//回车事件
    connect(ui->label_edit, &CustomizeEdit::textChanged, this, &ApplyFriend::SlotLabelTextChange);//文本变更
    connect(ui->label_edit, &CustomizeEdit::editingFinished, this, &ApplyFriend::SlotLabelEditFinished);//编辑完成
    connect(ui->tip_label, &ClickedOnceLabel::clicked, this, &ApplyFriend::SlotAddFirendLabelByClickTip);//提示标签点击（连续添加）

    //隐藏水平和垂直的滑动条
    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);//安装事件过滤器
    ui->sure_btn->SetState("normal","hover","press");
    ui->cancel_btn->SetState("normal","hover","press");
    //连接确认和取消按钮的槽函数
    connect(ui->cancel_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplyCancel);
    connect(ui->sure_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplySure);

}

ApplyFriend::~ApplyFriend()
{
    qDebug()<< "ApplyFriend was destructed";
    delete ui;
}

//初始化提示标签
void ApplyFriend::InitTipLabels()
{
    int lines = 1;
    for(std::size_t i = 0; i < _tip_data.size(); i++)
    {

        auto* label = new ClickedLabel(ui->label_list);
        label->SetState("normal",          "hover",          "pressed",
                        "selected_normal", "selected_hover", "selected_pressed");
        label->setObjectName("tipslabel");
        label->setText(_tip_data[i]);
        connect(label, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(label->font()); // 获取QLabel控件的字体信息
        //获取文本的宽度
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

// 添加提示标签，PS: next_point为一个回调参数，由此函数外部创建获得
void ApplyFriend::AddTipLabels(ClickedLabel *label, QPoint cur_point, QPoint &next_point, int text_width, int text_height)
{
    Q_UNUSED(text_height);

    label->move(cur_point);
    label->show();
    _add_labels.insert(label->text(), label);
    _add_label_keys.push_back(label->text());
    next_point.setX(label->pos().x() + text_width + 15);
    next_point.setY(label->pos().y());
}

bool ApplyFriend::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->scrollArea && event->type() == QEvent::Enter)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if(obj == ui->scrollArea && event->type() == QEvent::Leave)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(obj, event);
}

void ApplyFriend::SetSearchInfo(std::shared_ptr<SearchInfo> search_info)
{
    _search_info = search_info;
    QString applyname = UserMgr::GetInstance()->GetName();
    QString backname = search_info->_name;//昵称（备注）
    ui->name_edit->setText(applyname);
    ui->back_edit->setText(backname);
}

void ApplyFriend::resetLabels()
{
    auto max_width = ui->grid_wid->width();
    auto label_height = 0;
    for(auto iter = _friend_labels.begin(); iter != _friend_labels.end(); iter++){
        //添加宽度统计
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

void ApplyFriend::addLabel(QString name)
{
    // _friend_labels 中未找到 name
    if(_friend_labels.find(name) != _friend_labels.end())
    {
        return;
    }

    auto templabel = new FriendLabel(ui->grid_wid);
    templabel->SetText(name);
    templabel->setObjectName("FriendLabel");

    auto max_width = ui->grid_wid->width();
    //to do 添加宽度统计
    if(_label_point.x() + templabel->width() > max_width)
    {
        _label_point.setX(2);
        _label_point.setY(_label_point.y() + templabel->Height() + 6);
    }
    else
    {

    }

    templabel->move(_label_point);
    templabel->show();
    _friend_labels[templabel->Text()] = templabel;
    _friend_label_keys.push_back(templabel->Text());

    connect(templabel, &FriendLabel::sig_close, this, &ApplyFriend::SlotRemoveFriendLabel);

    _label_point.setX(_label_point.x() + templabel->width() + 2);

    //添加新标签之后 lineEdit 起始输入偏移
    if(_label_point.x() + MIN_APPLY_LABEL_ED_LEN  > ui->grid_wid->width())
    {
        ui->label_edit->move(2, _label_point.y() + templabel->height() + 2);//换行
    }
    else
    {
        ui->label_edit->move(_label_point);
    }

    ui->label_edit->clear();

    if(ui->grid_wid->height() < _label_point.y() + templabel->height() +2 )
    {
        ui->grid_wid->setFixedHeight(_label_point.y() + templabel->height()*2  + 2);
    }
}

void ApplyFriend::ShowMoreLabel()
{
    qDebug()<< "receive more label clicked";
    ui->more_label_wid->hide();

    ui->label_list->setFixedWidth(325);
    _tip_cur_point = QPoint(5, 5);
    auto next_point = _tip_cur_point;
    int textWidth;
    int textHeight;
    //重排现有的 label
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
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }
        added_label->move(_tip_cur_point);

        next_point.setX(added_label->pos().x() + textWidth + 15);
        next_point.setY(_tip_cur_point.y());

        _tip_cur_point = next_point;
    }

    //添加未添加的
    for(size_t i = 0; i < _tip_data.size(); i++)
    {
        auto iter = _add_labels.find(_tip_data[i]);
        if(iter != _add_labels.end())
        {
            continue;
        }

        auto* label = new ClickedLabel(ui->label_list);
        label->SetState("normal", "hover", "pressed",
                        "selected_normal", "selected_hover", "selected_pressed");
        label->setObjectName("tipslabel");
        label->setText(_tip_data[i]);
        connect(label, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(label->font()); // 获取QLabel控件的字体信息
        // 获取文本的宽度
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                textWidth = fontMetrics.width(label->text);
        #else
                textWidth = fontMetrics.horizontalAdvance(label->text());
        #endif
        // 获取文本的高度
        textHeight = fontMetrics.height();

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->label_list->width())
        {
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
    ui->scrollContent->setFixedHeight(ui->scrollContent->height() + diff_height);
}

//输入label按下回车触发将标签加入展示栏
void ApplyFriend::SlotLabelEnter()
{
    //label_edit为空，直接返回
    if(ui->label_edit->text().isEmpty())
    {
        return;
    }
    //不为空，添加标签
    addLabel(ui->label_edit->text());
    ui->input_tip_wid->hide();

    auto text = ui->label_edit->text();
    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    //找到就设置状态为选中
    if(find_it == _tip_data.end())
    {
        _tip_data.push_back(text);
    }

    //判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(text);
    //找到
    if(find_add != _add_labels.end())
    {
        find_add.value()->SetCurState(ClickLabelState::Selected);
        return;
    }
    //没找到，执行添加逻辑
    //标签展示栏也增加一个标签, 并设置绿色选中
    auto* label = new ClickedLabel(ui->label_list);
    label->SetState("normal", "hover", "pressed",
                    "selected_normal", "selected_hover", "selected_pressed");
    label->setObjectName("tipslabel");
    label->setText(text);
    connect(label, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->label_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();
    QFontMetrics fontMetrics(label->font()); // 获取QLabel控件的字体信息
    // 获取文本的宽度
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        int textWidth = fontMetrics.width(label->text);
    #else
        int textWidth = fontMetrics.horizontalAdvance(label->text());
    #endif
    // 获取文本的高度
    int textHeight = fontMetrics.height();
    qDebug() << "textWidth is " << textWidth;

    //超过位置
    if (_tip_cur_point.x() + textWidth+ tip_offset + 3 > ui->label_list->width())
    {
        //设置为新一行的行首
        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
    }

    //没超过位置，直接在行末尾添加
    auto next_point = _tip_cur_point;
    AddTipLabels(label, _tip_cur_point, next_point, textWidth, textHeight);
    _tip_cur_point = next_point;//记录新添加标签后的坐标点

    int diff_height = next_point.y() + textHeight + tip_offset - ui->label_list->height();
    ui->label_list->setFixedHeight(next_point.y() + textHeight + tip_offset);//扩充 label_list 高度
    label->SetCurState(ClickLabelState::Selected);
    ui->scrollContent->setFixedHeight(ui->scrollContent->height() + diff_height );
}

//点击关闭，移除展示栏好友便签
void ApplyFriend::SlotRemoveFriendLabel(QString name)
{
    qDebug() << "receive close signal";

    _label_point.setX(2);
    _label_point.setY(6);

    auto find_iter = _friend_labels.find(name);

    if(find_iter == _friend_labels.end()){
        return;
    }

    auto find_key = _friend_label_keys.end();
    for(auto iter = _friend_label_keys.begin(); iter != _friend_label_keys.end(); iter++){
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

// 点击标签已有添加或删除新联系人的标签
void ApplyFriend::SlotChangeFriendLabelByTip(QString labeltext, ClickLabelState state)
{
    auto find_iter = _add_labels.find(labeltext);
    if(find_iter == _add_labels.end())
    {
        return;
    }

    if(state == ClickLabelState::Selected){
        //编写添加逻辑
        addLabel(labeltext);
        return;
    }

    if(state == ClickLabelState::Normal){
        //编写删除逻辑
        SlotRemoveFriendLabel(labeltext);
        return;
    }
}

// 标签文本内容变更
void ApplyFriend::SlotLabelTextChange(const QString &text)
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

// 标签编辑完成
void ApplyFriend::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void ApplyFriend::SlotAddFirendLabelByClickTip(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1)
    {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);

    auto find_it = std::find(_tip_data.begin(), _tip_data.end(), text);
    // 找到了就只需设置状态为选中即可
    if (find_it == _tip_data.end())
    {
        _tip_data.push_back(text);
    }

    // 判断标签展示栏是否有该标签
    auto find_add = _add_labels.find(text);
    if (find_add != _add_labels.end())
    {
        find_add.value()->SetCurState(ClickLabelState::Selected);
        return;
    }

    // 标签展示栏也增加一个标签, 并设置绿色选中
    auto* label = new ClickedLabel(ui->label_list);
    label->SetState("normal", "hover", "pressed",
                    "selected_normal", "selected_hover", "selected_pressed");
    label->setObjectName("tipslabel");
    label->setText(text);
    connect(label, &ClickedLabel::clicked, this, &ApplyFriend::SlotChangeFriendLabelByTip);
    qDebug() << "ui->lb_list->width() is " << ui->label_list->width();
    qDebug() << "_tip_cur_point.x() is " << _tip_cur_point.x();

    QFontMetrics fontMetrics(label->font()); // 获取QLabel控件的字体信息
    // 获取文本的宽度
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        int textWidth = fontMetrics.width(label->text);
    #else
        int textWidth = fontMetrics.horizontalAdvance(label->text());
    #endif
    // 获取文本的高度
    int textHeight = fontMetrics.height();


    qDebug() << "textWidth is " << textWidth;

    if (_tip_cur_point.x() + textWidth+ tip_offset+3 > ui->label_list->width())
    {
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

// 按下确认按钮
void ApplyFriend::SlotApplySure()
{
    qDebug()<<"Slot Apply Sure called" ;

    //发送添加请求消息体
    QJsonObject jsonObj;
    //1.设置uid
    int uid = UserMgr::GetInstance()->GetUid();
    jsonObj["uid"] = uid;
    //2.设置name 自己的名字
    QString name = ui->name_edit->text();
    if(name.isEmpty())
    {
        name = ui->name_edit->placeholderText();
    }
    //3.设置applyname 自己的名字
    jsonObj["applyname"] = name;
    //4.设置backname 备注名，把对方备注的名字
    QString backname = ui->back_edit->text();
    if(backname.isEmpty())
    {
        backname = ui->back_edit->placeholderText();
    }
    jsonObj["backname"] = backname;
    //5.设置touid 对方的uid
    jsonObj["touid"] = _search_info->_uid;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    //发送tcp请求给chatserver
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_ADD_FRIEND_REQ, jsonData);

    this->hide();
    deleteLater();
}

// 按下取消按钮
void ApplyFriend::SlotApplyCancel()
{
    qDebug() << "Slot Apply Cancel";
    this->hide();
    deleteLater();
}
