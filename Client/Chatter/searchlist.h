#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>
#include <QDialog>
#include <memory>
#include "userdata.h" //用户信息
#include "loadingdialog.h"

/******************************************************************************
 *
 * @file       searchlist.h
 * @brief      Function:搜索列表
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class SearchList : public QListWidget
{
    Q_OBJECT

public:
    SearchList(QWidget *parent = nullptr);
    ~SearchList();

    void CloseFindDialog();                // 关闭查找对话框
    void SetSearchEdit(QWidget *edit);     // 设置搜素框

protected:
    bool eventFilter(QObject *watched, QEvent *event) override; // 重写事件过滤器

private:
    void waitPending(bool pending = true);    // 等待阻塞
    void addTipItem();                        // 添加列表提示
    bool isPureDigit(const std::string& str); // 判断字符串是否为纯数字,用于区分名字和uid

signals:
    void sig_jump_chat_item(std::shared_ptr<SearchInfo> search_info); // 跳转聊天项

private slots:
    void slot_item_clicked(QListWidgetItem *item);                  // 某一Item点击弹窗处理
    void slot_user_search(std::shared_ptr<SearchInfo> search_info); // 用户搜索

private:
    bool _send_pending;                 // 发送阻塞
    std::shared_ptr<QDialog> _find_dlg; // 添加好友查找界面
    QWidget* _search_edit;              // 搜索框缓存
    LoadingDialog* _loadingDialog;      // 加载等待页
};

#endif // SEARCHLIST_H
