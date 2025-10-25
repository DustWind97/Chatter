#ifndef FINDSUCCESSDIALOG_H
#define FINDSUCCESSDIALOG_H

#include <QDialog>
#include <memory>
#include "userdata.h"

/******************************************************************************
 *
 * @file       findsuccessdialog.h
 * @brief      Function: 搜索检索结果弹框显示
 *
 * @author     DustWind丶
 * @date       2025/05/23
 * @history
 *****************************************************************************/
namespace Ui {
class FindSuccessDialog;
}

class FindSuccessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDialog(QWidget *parent = nullptr);
    ~FindSuccessDialog();

    void SetSearchInfo(std::shared_ptr<SearchInfo> search_info);//设置搜索信息

private slots:
    void on_add_friend_btn_clicked();//添加好友

private:
    Ui::FindSuccessDialog *ui;

    std::shared_ptr<SearchInfo> _search_info;//SearchInfo信息缓存
    QWidget * _parent;//保存父指针，透传数据给父类
};

#endif // FINDSUCCESSDIALOG_H
