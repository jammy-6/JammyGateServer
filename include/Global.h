#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <iostream>
#include <map>
#include <string>
#include <queue>

#include "log.h"
#include "Config.h"


enum ERRORCODE {
    Success = 0,
    Error_Json = 1001,  //Json解析错误
    RPCFailed = 1002,  //RPC请求错误
    Password_Not_Equal = 1003, //密码不一致
    Varify_Code_Expired = 1004, //验证码失效
    Varify_Code_Not_Equal = 1005, //验证码不一致
    Error_User_Exist = 1006,///用户已存在
};

const std::string REDIS_EMAIL_CODE_PREFIX = "code_";

#endif