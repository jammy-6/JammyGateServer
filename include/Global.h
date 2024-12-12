#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <iostream>
#include <map>
#include <string>
#include <queue>

#include "log.h"
#include "Config.h"

typedef struct UserInfo{

    int uid;
    std::string token;
    std::string host;
    std::string name;
    std::string email;
    std::string pwd;
}UserInfo;

enum ERRORCODE {
    Success = 0,
    Error_Json = 1001,  //Json解析错误
    RPCFailed = 1002,  //RPC请求错误
    Password_Not_Equal = 1003, //密码不一致
    Varify_Code_Expired = 1004, //验证码失效
    Varify_Code_Not_Equal = 1005, //验证码不一致
    Error_User_Exist = 1006,///用户已存在
    Error_User_Not_Exist = 1007 ,//用户不存在
    Error_Update_Password = 1008,///更新数据库失败
    ERROR_PASSWORD_NOT_CORRECT = 1009,///密码不正确
    ERROR_USER_EMAIL_NOT_MATCH = 1010,//用户名与邮箱不匹配
    
};

const std::string REDIS_EMAIL_CODE_PREFIX = "code_";

#endif