#ifndef GLOBAL_H
#define GLOBAL_H

/************************************************************
* 添加常用的一些头文件，这样不必每次在写代码是重复添加头文件
* 当然应当慎重使用这一方式，通常是在哪个类使用就在哪儿引用头文件
************************************************************/
#include <QWidget>
#include <functional>//函数式编程
#include "QStyle"
#include <QRegularExpression>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>
#include <QDir>
#include <QSettings>
#include <QCryptographicHash>   //进行哈希算法对密码散列加密
#include <QString>
#include <QDebug>
#include <QPixmap>

/******************************************************************************
 *
 * @file       global.h
 * @brief      Function: 全局配置
 *
 * @author     DustWind丶
 * @date       2024/07/26
 * @history
 *****************************************************************************/
extern std::function<void(QWidget*)>   repolish;   //qss属性刷新
extern std::function<QString(QString)> xorString;  //异或字符串，实际并没使用
extern std::function<QString(QString)> md5Encrypt; //MD5密码映射函数来进行密文传输

//网络请求码
enum ReqId{
    ID_GET_VERIFY_CODE          = 1001, // 获取验证码
    ID_REG_USER                 = 1002, // 注册用户
    ID_RESET_PWD                = 1003, // 重置密码
    ID_LOGIN_USER               = 1004, // 用户登录（HTTP连接）
    ID_CHAT_LOGIN               = 1005, // 登陆聊天服务器（TCP连接）
    ID_CHAT_LOGIN_RSP           = 1006, // 登陆聊天服务器回包
    ID_SEARCH_USER_REQ          = 1007, // 用户搜索请求
    ID_SEARCH_USER_RSP          = 1008, // 搜索用户回包
    ID_ADD_FRIEND_REQ           = 1009, // 添加好友申请
    ID_ADD_FRIEND_RSP           = 1010, // 申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ    = 1011, // 通知用户添加好友申请
    ID_AUTH_FRIEND_REQ          = 1013, // 认证好友请求
    ID_AUTH_FRIEND_RSP          = 1014, // 认证好友回复 服务器回复
    ID_NOTIFY_AUTH_FRIEND_REQ   = 1015, // 通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ        = 1017, // 文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP        = 1018, // 文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 通知用户文本聊天信息

    ID_NOTIFY_OFF_LINE_REQ      = 1021, // 通知用户下线
    ID_HEART_BEAT_REQ           = 1023, // 心跳请求
    ID_HEART_BEAT_RSP           = 1024, // 心跳回复
};

//提示错误（输入合法性检验）
enum TipErr{
    TIP_SUCCESS     = 0, //成功码
    TIP_EMAIL_ERR   = 1, //邮箱格式错误
    TIP_PWD_ERR     = 2, //密码格式错误
    TIP_CONFIRM_ERR = 3, //确认密码格式错误
    TIP_PWD_CONFIRM = 4, //密码与确认提示
    TIP_VERIFY_ERR  = 5, //验证码错误
    TIP_USER_ERR    = 6  //用户信息错误
};

//模块
enum Modules{
    REGISTERMOD = 0, //注册模块
    RESETMOD    = 1, //重置密码模块
    LOGINMOD    = 2, //登录模块
};

//错误码
enum ErrorCodes{
    SUCCESS     = 0, //成功码
    ERR_JSON    = 1, //json解析错误
    ERR_NETWORK = 2, //网络错误
};

//可点击标签状态
enum ClickLabelState
{
    Normal   = 0, //普通模式
    Selected = 1, //选中模式
};

//服务器信息
struct ServerInfo{
    QString Host;  //IP
    QString Port;  //端口
    QString Token; //Token
    int Uid;       //UID
};

//聊天对象
enum class ChatRole
{
    Self,  //自己
    Other, //他人
};

//聊天消息结构
struct MsgInfo{
    QString msgFlag; //"text,image,file等"
    QString content; //表示文件和图像的url,文本信息
    QPixmap pixmap;  //文件和图片的缩略图
};

//聊天界面几种模式
enum ChatUIMode{
    SearchMode,  //搜索模式
    ChatMode,    //聊天模式
    ContactMode, //联系模式
};

//自定义QListWidgetItem的几种类型
enum ListItemType{
    CHAT_USER_ITEM,    //聊天用户条目
    CONTACT_USER_ITEM, //联系人用户条目
    SEARCH_USER_ITEM,  //搜索到的用户条目
    ADD_USER_TIP_ITEM, //提示添加用户条目
    INVALID_ITEM,      //不可点击条目
    GROUP_TIP_ITEM,    //分组提示条目
    LINE_ITEM,         //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

/*以下为测试模拟数据***********************************************************************************************************/
const std::vector<QString> names = {
    "c++",
    "golang",
    "swift",
    "java",
    "nodejs",
    "python",
    "rust"
};
const std::vector<QString> heads = {
    ":/res/head_1.jpg",
    ":/res/head_2.jpg",
    ":/res/head_3.jpg",
    ":/res/head_4.jpg",
    ":/res/head_5.jpg"
};
const std::vector<QString> msgs ={"Hello, World!",
                             "Nice to meet u",
                             "New year，new life",
                             "You have to love yourself",
                             "My love is written in the wind ever since the whole world is you",
                             "Do not go gentle into that good night"};
/*以上为测试模拟数据***********************************************************************************************************/

//申请好友标签输入框最低长度
const int MIN_APPLY_LABEL_ED_LEN = 40; //设定申请好友编辑框长度为40
const QString add_prefix = "添加标签 "; //添加标签前缀
const int tip_offset = 5;              //提示便宜
const int CHAT_COUNT_PER_PAGE = 13;    //聊天页列表每页数量
extern QString gate_url_prefix;        //网络请求前缀

#endif // GLOBAL_H
