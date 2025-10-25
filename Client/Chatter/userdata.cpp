#include "userdata.h"

SearchInfo::SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon)
    :_uid(uid)
    ,_name(name)
    , _nick(nick)
    ,_desc(desc)
    ,_sex(sex)
    ,_icon(icon)
{

}

SearchInfo::~SearchInfo()
{
    qDebug() << "SearchInfo was destructed";
}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc,
                               QString icon, QString nick, int sex)
    : _from_uid(from_uid)
    , _name(name)
    , _desc(desc)
    , _icon(icon)
    , _nick(nick)
    , _sex(sex)
{

}

AddFriendApply::~AddFriendApply()
{
    qDebug() << "AddFriendApply was destructed";
}

void FriendInfo::AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData> > text_vec)
{
    for(const auto & text: text_vec){
        _chat_msgs.push_back(text);
    }
}
