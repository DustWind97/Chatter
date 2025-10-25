#ifndef APPLYFRIENDLIST_H
#define APPLYFRIENDLIST_H

#include <QListWidget>
#include <QEvent>

/******************************************************************************
 *
 * @file       applyfriendlist.h
 * @brief      Function: apply_friend_list方便继承
 *
 * @author     DustWind丶
 * @date       2025/06/05
 * @history
 *****************************************************************************/
class ApplyFriendList: public QListWidget
{
    Q_OBJECT

public:
    ApplyFriendList(QWidget *parent = nullptr);
    ~ApplyFriendList();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void sig_show_search(bool);

private slots:

};

#endif // APPLYFRIENDLIST_H
