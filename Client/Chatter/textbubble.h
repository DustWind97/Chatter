#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

#include "bubbleframe.h"
#include <QTextEdit>
#include <QHBoxLayout>
#include <QEvent>

/******************************************************************************
 *
 * @file       textbubble.h
 * @brief      Function: 文本气泡框
 *
 * @author     DustWind丶
 * @date       2025/05/11
 * @history
 *****************************************************************************/
class TextBubble : public BubbleFrame
{
    Q_OBJECT

public:
    TextBubble(ChatRole role, const QString &text, QWidget *parent = nullptr);
    ~TextBubble();

public:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;//事件过滤器，调节文本高度，初始化样式

private:
    void adjustTextHeight();
    void setPlainText(const QString &text);
    void initStyleSheet();

private:
    QTextEdit *m_pTextEdit;
};

#endif // TEXTBUBBLE_H
