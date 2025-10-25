#include "global.h"

QString gate_url_prefix="";

std::function<void(QWidget*)> repolish = [](QWidget* w)
{
    w->style()->unpolish(w);
    w->style()->polish(w);
};

//暂时先采用异或操作对密码进行处理
std::function<QString(QString)> xorString = [](QString input)
{
    QString result = input;//复制原字符串方便后续处理
    int length = input.length();
    int xorlength = length % 255;
    for (int i = 0; i < length; ++i)
    {
        //对每个字符进行异或操作
        //PS，这里假设均为ASCII码，因此直接转换为QChar
        result[i] = QChar(static_cast<ushort>(input[i].unicode()^static_cast<ushort>(xorlength)));
    }
    return result;
};

//后续修改为MD5散列
std::function<QString(QString)> md5Encrypt = [](QString input)
{
    QByteArray inputData = input.toUtf8(); // 将QString转换为QByteArray，因为QCryptographicHash需要QByteArray作为输入
    QCryptographicHash hash(QCryptographicHash::Md5); // 创建使用MD5算法的哈希对象
    hash.addData(inputData); // 添加数据以计算哈希值
    QByteArray hashBytes = hash.result(); // 获取哈希值
    QString resultString = hashBytes.toHex(); // 将哈希值转换为十六进制字符串形式
    return resultString;
};
