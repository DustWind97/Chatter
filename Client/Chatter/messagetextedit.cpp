#include "messagetextedit.h"
#include <QChar>
#include <QMessageBox>
#include <QStringList>
#include <QPainter>
#include <QFileIconProvider>

//有参构造
MessageTextEdit::MessageTextEdit(QWidget *parent)
    :QTextEdit(parent)
{
    this->setMaximumHeight(60);
}

//析构
MessageTextEdit::~MessageTextEdit()
{

}

//获取消息列表
QVector<MsgInfo> MessageTextEdit::getMsgList()
{
    _mGetMsgList.clear();

    QString doc = this->document()->toPlainText();
    QString text = "";//存储文本信息
    int indexUrl = 0;
    int count = _mMsgList.size();

    for (int index = 0; index < doc.size(); index++) {
        if(doc[index] == QChar::ObjectReplacementCharacter)
        {
            if(!text.isEmpty())
            {
                QPixmap pix;
                insertMsgList(_mGetMsgList, "text", text, pix);
                text.clear();
            }
            while (indexUrl<count) {
                MsgInfo msg = _mMsgList[indexUrl];
                if(this->document()->toHtml().contains(msg.content,Qt::CaseSensitive))//区分大小写
                {
                    indexUrl++;
                    _mGetMsgList.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else
        {
            text.append(doc[index]);
        }
    }
    if(!text.isEmpty())
    {
        QPixmap pix;
        insertMsgList(_mGetMsgList, "text", text, pix);
        text.clear();
    }
    _mMsgList.clear();
    this->clear();
    return _mGetMsgList;
}

//从链接中插入文件
void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    //如果为空，直接返回
    if(urls.isEmpty())
    {
        return;
    }

    //遍历urls，执行对应操作
    foreach (QString url, urls) {
        if(isImage(url))
        {
            insertImages(url);
        }
        else
        {
            insertTextFile(url);
        }
    }
}

//拖动进入事件
void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if( event->source() == this)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

//放下事件
void MessageTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

//按键事件
void MessageTextEdit::keyPressEvent(QKeyEvent *event)
{
    //检测键盘是否按下了Enter键或者Return键，并且没有同时按住Shift键
    if((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && !(event->modifiers() & Qt::ShiftModifier))
    {
        emit send();
        event->accept();//标记已处理
        return;
    }
    // if(event->key() == Qt::Key_Paste)//粘贴
    // {

    // }
    QTextEdit::keyPressEvent(event);
}

//插入图片
void MessageTextEdit::insertImages(const QString &url)
{
    QImage image(url);
    //按比例缩放图片
    if(image.width() > 120 || image.height() > 80)
    {
        if(image.width() > image.height())
        {
            image = image.scaledToWidth(120, Qt::SmoothTransformation);
        }
        else
        {
            image = image.scaledToHeight(80, Qt::SmoothTransformation);
        }
    }

    QTextCursor cursor = this->textCursor();//获取文本光标
    cursor.insertImage(image, url);
    insertMsgList(_mMsgList, "image", url, QPixmap::fromImage(image));//添加消息列表
}

//插入文本
void MessageTextEdit::insertTextFile(const QString &url)
{
    QFileInfo fileInfo(url);
    if(fileInfo.isDir())
    {
        QMessageBox::information(this, "提示", "只允许拖曳单个文件");
        return;
    }

    if(fileInfo.size() > 100*1024*1024)//文件大小超过100M
    {
        QMessageBox::information(this, "提示", "发送文件大小不能超过100M");
        return;
    }

    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(), url);
    insertMsgList(_mMsgList, "file", url, pix);//添加消息列表
}

//判断是否可以插入多媒体数据
bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

//插入多媒体数据
void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());

    //如果为空，直接返回
    if(urls.isEmpty())
    {
        return;
    }

    //遍历urls，执行对应操作
    foreach (QString url, urls) {
        if(isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

//判断是否为图片
bool MessageTextEdit::isImage(QString url)
{
    //枚举图片格式列表
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    //分割图片格式列表
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);//获取文件信息
    QString suffix = fileInfo.suffix();
    //是列表中图片格式（大小写敏感）
    if(imageFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}

//插入消息列表
void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pic)//list为回调参数
{
    MsgInfo msg;
    msg.msgFlag = flag;
    msg.content = text;
    msg.pixmap = pic;
    list.append(msg);
}

//获取链接
QStringList MessageTextEdit::getUrl(QString text)
{
    QStringList urls;
    if(text.isEmpty())
        return urls;

    QStringList list = text.split("\n");
    foreach (QString url, list) {
        if(!url.isEmpty())
        {
            QStringList str = url.split("///");
            if(str.size() >= 2)
                urls.append(str.at(1));
        }
    }
    return urls;

    // //改进版本
    // QStringList urls;
    // if (text.isEmpty())
    //     return urls;

    // QStringList list = text.split("\n", Qt::SkipEmptyParts);

    // foreach (QString url, list) {
    //     if (url.isEmpty())
    //         continue;

    //     QString path;
    //     // 处理不同操作系统的文件URL前缀
    //     if (url.startsWith("file://")) {
    //         // 统一处理 "file:///" 或 "file://"
    //         path = url.mid(url.startsWith("file:///") ? 8 : 7);
    //         // URL解码（处理特殊字符如%20）
    //         path = QUrl::fromPercentEncoding(path.toUtf8());
    //     } else {
    //         path = url;
    //     }

    //     // 检查路径有效性
    //     QFileInfo fileInfo(path);
    //     if (!fileInfo.exists()) {
    //         qWarning() << "无效路径：" << path;
    //         continue;
    //     }

    //     urls.append(path);
    // }
    // return urls;
}

//获取Icon位图
QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    //qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"), 10, QFont::Normal,false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);//(0,0)点到（50,50）点
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

//获取文件尺寸
QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;//单位
    double num;//文件大小的数值
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;//将num固定两位小数点
}

//TextEdit更新更改
void MessageTextEdit::textEditChanged()
{
    //qDebug() << "text changed!";
}

