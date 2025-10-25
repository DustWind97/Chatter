#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include <QDialog>
#include "clickedlabel.h"
#include "friendlabel.h"
#include "userdata.h"

/******************************************************************************
 *
 * @file       applyfriend.h
 * @brief      Function: 添加好友Dialog
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();

    void InitTipLabels();//测试用
    void AddTipLabels(ClickedLabel *label, QPoint cur_point, QPoint &next_point, int text_width, int text_height);//添加标签
    bool eventFilter(QObject *obj, QEvent *event);//事件过滤器
    void SetSearchInfo(std::shared_ptr<SearchInfo> search_info);//设置信息

private:
    void resetLabels();//重置所有 label
    void addLabel(QString name);//添加标签

public slots:
    void ShowMoreLabel();//显示更多label标签
    void SlotLabelEnter();//输入label按下回车触发将标签加入展示栏
    void SlotRemoveFriendLabel(QString name);//点击关闭，移除展示栏好友便签
    void SlotChangeFriendLabelByTip(QString text, ClickLabelState state);//通过点击tip实现增加和减少好友便签
    void SlotLabelTextChange(const QString& text);//输入框文本变化显示不同提示
    void SlotLabelEditFinished();//输入框输入完成
    void SlotAddFirendLabelByClickTip(QString text);//输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotApplySure();//确认按钮，涉及网络请求
    void SlotApplyCancel();//取消按钮

private:
    Ui::ApplyFriend *ui;

    QMap<QString, ClickedLabel*> _add_labels;//已经创建好的标签
    std::vector<QString> _add_label_keys;
    QPoint _label_point;//每次添加计算在哪个点添加
    //用来在输入框中显示添加新朋友的标签
    QMap<QString, FriendLabel*> _friend_labels;
    std::vector<QString> _friend_label_keys;
    std::vector<QString> _tip_data;//输入时在下方有提示
    QPoint _tip_cur_point;//记录当前提示的位置
    std::shared_ptr<SearchInfo> _search_info;
};

#endif // APPLYFRIEND_H
