#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"
#include "userdata.h"
#include <QListWidgetItem>

/******************************************************************************
 *
 * @file       chatdialog.h
 * @brief      Function: 聊天界面
 *
 * @author     DustWind丶
 * @date       2025/04/19
 * @history
 *****************************************************************************/
namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

protected:
    //重写基类的函数尽量都用protected
    bool eventFilter(QObject* wanted, QEvent* event) override;
    void handleGlobalMousePress(QMouseEvent *event);//全局处理鼠标点击事件

public slots:
    void slot_loading_chat_user();//加载聊天列表
    void slot_loading_contact_user();//加载联系人列表
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString &str);
    void slot_show_search(bool b_show);

    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply_info);
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> search_info);           // 通过搜索跳转好友聊天页
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info); // 通过好友信息页跳转好友聊天页
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);             // 好友信息页
    void slot_switch_apply_friend_page();                                        // 切换好友申请页
    void slot_item_clicked(QListWidgetItem* item);                               // 点击聊天列表某一项
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);       // 本地待发送消息处理
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);                   // 接收对端发送来的消息

private:
    void ShowSearch(bool bsearch = false);    // 展示搜索页，默认为false
    void AddChatUserList();                   // 添加聊天列表
    void AddLabelGroup(StateWidget *label);   // 添加添加标签组
    void ClearLabelState(StateWidget *label); // 清空标签状态
    void SetSelectChatItem(int uid = 0);      // 设置选中聊天项
    void SetSelectChatPage(int uid = 0);      // 设置选中聊天页信息
    void LoadMoreChatUser();                  // 加载更多聊天用户
    void LoadMoreContactUser();               // 加载更多联系人
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgs); //更新当前页面聊天记录

private:
    Ui::ChatDialog *ui;
    //模式和状态是通过两种方式实现点击切换
    ChatUIMode _mode;//控制sidebar
    ChatUIMode _state;//ChatUIMode模式控制
    bool _b_loading;//是否展示加载页面
    QList<StateWidget*> _label_list;//管理状态列表
    QMap<int, QListWidgetItem*> _chat_items_added;//已添加聊天项，通过uid映射界面
    int _cur_chat_uid;//当前正在聊天的好友对应的uid
    QWidget* _last_widget;//上次加载页面
};

#endif // CHATDIALOG_H
