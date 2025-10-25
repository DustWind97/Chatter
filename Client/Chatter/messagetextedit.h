#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include "global.h"
#include <QTextEdit>
#include <QMimeData>
#include <QDragEnterEvent>

/******************************************************************************
 *
 * @file       messagetextedit.h
 * @brief      Function: 发功消息栏的 TextEdit 重写功能
 *
 * @author     DustWind丶
 * @date       2025/06/03
 * @history
 *****************************************************************************/
class MessageTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit MessageTextEdit(QWidget *parent = nullptr);
    ~MessageTextEdit();

    QVector<MsgInfo> getMsgList();//获取消息列表
    void insertFileFromUrl(const QStringList &urls);

signals:
    void send();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;//拖动文件处理
    void dropEvent(QDropEvent *event) override;//放下处理
    void keyPressEvent(QKeyEvent *event) override;//按下回车的处理(消息书写快捷键)
    //todo... 后续写对此TextEdit的快捷键操作，如Ctrl+V的paste操作

private:
    void insertImages(const QString &url);//插入图片
    void insertTextFile(const QString &url);//插入文本
    bool isImage(QString url);//判断是否为图片
    void insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pic);
    QStringList getUrl(QString text);              //富文本的url
    QPixmap getFileIconPixmap(const QString &url); //获取文件图标及大小信息，并转化为图片
    QString getFileSize(qint64 size);              //获取文件大小

protected:
    virtual bool canInsertFromMimeData(const QMimeData *source) const override;//多媒体
    virtual void insertFromMimeData(const QMimeData *source) override;

private slots:
    void textEditChanged();

private:
    QVector<MsgInfo> _mMsgList;    // 消息列表
    QVector<MsgInfo> _mGetMsgList; // 获取消息列表
};

#endif // MESSAGETEXTEDIT_H
