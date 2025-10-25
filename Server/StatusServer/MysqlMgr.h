#pragma once
#include "const.h"
#include "MysqlDao.h"

//此类只是接口调用层，将接口信息暴露出来，具体实现在DAO中操作
class MysqlMgr:public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& name, const std::string& pwd);
    bool CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo);
    
private:
    MysqlMgr();
    MysqlDao _dao;
};

