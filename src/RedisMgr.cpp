//
// Created by root on 24-10-29.
//
#include "RedisMgr.h"
#include "log.h"
RedisMgr::RedisMgr() {
    auto redisIP = gConfigMgr["RedisServer"]["Ip"];
    auto password = gConfigMgr["RedisServer"]["Password"];
    int port = static_cast<int>(stoi(gConfigMgr["RedisServer"]["Port"]));
    LOG_INFO("RedisMgr初始化成功,地址为 %s:%d，认证密码为%s", redisIP.c_str(),
             port, password.c_str());
    _conn_pool.reset(
        new RedisConPool(12, redisIP.c_str(), port, password.c_str()));
}

RedisMgr::~RedisMgr() { _conn_pool->Close(); }

bool RedisMgr::Get(const std::string &key, std::string &value) {
    auto connnection = _conn_pool->getConnection();
    if (connnection == nullptr) {
        return false;
    }
    auto reply = (redisReply *)redisCommand(connnection, "GET %s", key.c_str());
    if (reply == nullptr) {
        LOG_INFO("Execut command GET [key = %s] failed ", key.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connnection);
        return false;
    }
    if (reply->type != REDIS_REPLY_STRING) {
        LOG_INFO("Execut command GET [key = %s] failed ", key.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connnection);
        return false;
    }

    value = reply->str;
    freeReplyObject(reply);
    _conn_pool->returnConnection(connnection);
    return true;
}

bool RedisMgr::Set(const std::string &key, const std::string &value) {
    auto connection = _conn_pool->getConnection();
    // 执行redis命令行
    auto reply = (redisReply *)redisCommand(connection, "SET %s %s",
                                            key.c_str(), value.c_str());
    // 如果返回NULL则说明执行失败
    if (NULL == reply) {
        LOG_INFO("Execut command SET [key = %s,value=%s] failed ", key.c_str(),
                 value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    // 如果执行失败则释放连接
    if (!(reply->type == REDIS_REPLY_STATUS &&
          (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0))) {
        LOG_INFO("Execut command SET [key = %s,value=%s] failed ", key.c_str(),
                 value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    // 执行成功 释放redisCommand执行后返回的redisReply所占用的内存
    freeReplyObject(reply);
    LOG_INFO("Redis服务器插入 %s:%s 成功", key.c_str(), value.c_str());
    _conn_pool->returnConnection(connection);
    return true;
}

// bool RedisMgr::Auth(const std::string& password)
// {
// }

bool RedisMgr::LPush(const std::string &key, const std::string &value) {
    auto connection = _conn_pool->getConnection();
    auto reply = (redisReply *)redisCommand(connection, "LPUSH %s %s",
                                            key.c_str(), value.c_str());
    if (NULL == reply) {
        LOG_INFO("Execut command LPUSH [key = %s,value=%s] failed ",
                 key.c_str(), value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        LOG_INFO("Execut command LPUSH [key = %s,value=%s] failed ",
                 key.c_str(), value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

bool RedisMgr::LPop(const std::string &key, std::string &value) {
    auto connection = _conn_pool->getConnection();
    auto reply =
        (redisReply *)redisCommand(connection, "LPOP %s ", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        LOG_INFO("Execut command LPOP [key = %s] failed ", key.c_str(),
                 value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

bool RedisMgr::RPush(const std::string &key, const std::string &value) {
    auto connection = _conn_pool->getConnection();
    auto reply = (redisReply *)redisCommand(connection, "RPUSH %s %s",
                                            key.c_str(), value.c_str());
    if (NULL == reply) {
        LOG_INFO("Execut command RPUSH [key = %s] failed ", key.c_str(),
                 value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        LOG_INFO("Execut command RPUSH [key = %s] failed ", key.c_str(),
                 value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

bool RedisMgr::RPop(const std::string &key, std::string &value) {
    auto connection = _conn_pool->getConnection();
    auto reply =
        (redisReply *)redisCommand(connection, "RPOP %s ", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        LOG_INFO("Execut command RPOP [key = %s] failed ", key.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

bool RedisMgr::HSet(const std::string &key, const std::string &hkey,
                    const std::string &value) {
    auto connection = _conn_pool->getConnection();
    auto reply = (redisReply *)redisCommand(
        connection, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        LOG_INFO("Execut command HSet [key = %s] failed ", key.c_str(),
                 value.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

bool RedisMgr::HSet(const char *key, const char *hkey, const char *hvalue,
                    size_t hvaluelen) {
    auto connection = _conn_pool->getConnection();
    const char *argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    auto reply = (redisReply *)redisCommandArgv(connection, 4, argv, argvlen);
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        LOG_INFO("Execut command HSet [key = %s,hkey = %s,hvalue = %s] failed ",
                 key, hkey, hvalue);
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

std::string RedisMgr::HGet(const std::string &key, const std::string &hkey) {
    auto connection = _conn_pool->getConnection();
    const char *argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();
    auto reply = (redisReply *)redisCommandArgv(connection, 3, argv, argvlen);
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        LOG_INFO("Execut command HGet [key = %s, hkey = %s] failed ",
                 key.c_str(), hkey.c_str());
        return "";
    }
    std::string value = reply->str;
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return value;
}

bool RedisMgr::Del(const std::string &key) {
    auto connection = _conn_pool->getConnection();
    auto reply = (redisReply *)redisCommand(connection, "DEL %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        LOG_INFO("Execut command Del [key = %s] failed ", key.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

bool RedisMgr::ExistsKey(const std::string &key) {
    auto connection = _conn_pool->getConnection();
    auto reply =
        (redisReply *)redisCommand(connection, "exists %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER ||
        reply->integer == 0) {
        LOG_INFO("Execut command EXISTS [key = %s], result not exists ",
                 key.c_str());
        freeReplyObject(reply);
        _conn_pool->returnConnection(connection);
        return false;
    }
    freeReplyObject(reply);
    _conn_pool->returnConnection(connection);
    return true;
}

void RedisMgr::Close() { _conn_pool->Close(); }
