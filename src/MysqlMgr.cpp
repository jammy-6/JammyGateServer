#include "MysqlMgr.h"
MysqlMgr::~MysqlMgr() {
}
int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    return _dao.RegUser(name, email, pwd);
}
bool MysqlMgr::checkUserExist(const std::string& name, const std::string& email){
    return _dao.checkUserExist(name, email);
}

bool MysqlMgr::checkUserEmailMatch(const std::string& user,const std::string& email){
    return _dao.checkUserEmailMatch(user, email);
}
bool MysqlMgr::updatePassword(const std::string& name, const std::string& email,const std::string& newPasswd){
    return _dao.updatePassword(name, email,newPasswd);
}
bool MysqlMgr::checkUserPassword(const std::string& name, const std::string& passwd,UserInfo& userInfo){
    return _dao.checkUserPassword(name,passwd,userInfo);
}
MysqlMgr::MysqlMgr() {
}