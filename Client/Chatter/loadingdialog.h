#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QDialog>

/******************************************************************************
 *
 * @file       loadingdialog.h
 * @brief      Function: 加载界面
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
namespace Ui {
class LoadingDialog;
}

class LoadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingDialog(QWidget *parent = nullptr);
    ~LoadingDialog();

private:
    Ui::LoadingDialog *ui;
};

#endif // LOADINGDIALOG_H
