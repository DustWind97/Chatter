#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include "userdata.h"
#include <QMap>

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

    void SetUserInfo(std::shared_ptr<UserInfo> user_info);  // 设置用户信息
    void AppendChatMsg(std::shared_ptr<TextChatData> msg);  // 添加聊天信息

protected:
    virtual void paintEvent(QPaintEvent *event) override;

signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData> msg);

private slots:
    void on_send_btn_clicked();
    void on_receive_btn_clicked();

private:
    void clearItems();

private:
    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> _user_info; //用户信息
    QMap<QString, QWidget*>  _bubble_map; //气泡框/*uid 与 bubble绑定*/
};
#endif // CHATPAGE_H
