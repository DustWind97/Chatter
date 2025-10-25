#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H
#include <QLineEdit>

/******************************************************************************
 *
 * @file       customizeedit.h
 * @brief      Function: 重写QLineEdit控件
 *
 * @author     DustWind丶
 * @date       2025/04/19
 * @history
 *****************************************************************************/
class CustomizeEdit:public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent = nullptr);
    ~CustomizeEdit();

    void SetMaxLength(int maxlen);

protected:
    void focusOutEvent(QFocusEvent *event) override; //重写失去焦点事件

signals:
    void sig_focus_out();

private:
    void limitTextLenght(QString text);//设置单行最大长度,仅显示_max_len长度的内容，对于多出的部分则隐藏
    int _max_len; //单行最大长度
};

#endif // CUSTOMIZEEDIT_H
