//
// Created by root on 24-10-30.
//

#ifndef MYSQLDAO_H
#define MYSQLDAO_H
#include "MysqlPool.h"
#include "ConfigMgr.h"
#include "Global.h"

class MysqlDao{
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
    std::unique_ptr<MysqlPool> pool_;

};
#endif //MYSQLDAO_H
