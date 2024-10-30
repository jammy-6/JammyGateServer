#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"
#include "nlohmann/json.hpp"
#include "MysqlMgr.h"
using json = nlohmann::json;

void LogicSystem::RegGet(std::string url, HttpHandler handler) {
    _get_handlers.insert(make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler) {
    _post_handlers.insert(make_pair(url, handler));
}
LogicSystem::LogicSystem() {
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->_response.body()) << "receive get_test req";
    });

    RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        LOG_INFO("receive body is %s ", body_str.c_str());
        connection->_response.set(http::field::content_type, "text/json");

        json body_json;
        json response_json;
        try{
            body_json = json::parse(body_str);
        }catch (json::parse_error& e){
            LOG_ERROR( "Failed to parse JSON data!");
             response_json["error_code"] = ERRORCODE::Error_Json;
             response_json["body"] = body_str;
            beast::ostream(connection->_response.body()) << response_json;
            return true;
        }
       std::string email = body_json["email"];

       GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
        auto err  = rsp.error();
        LOG_INFO("receive grpc, size = %d, rsp = %s",rsp.DebugString().size(),rsp.DebugString().c_str());
       LOG_INFO( "email is %s" ,  rsp.email().c_str());
       LOG_INFO( "code is %s" , rsp.code().c_str());
        LOG_INFO( "error is %d" , err);
       response_json["error_code"] = 0;
       response_json["email"] = rsp.email();
       response_json["code"] = rsp.code();
    
       beast::ostream(connection->_response.body()) << response_json.dump();
       return true;
    });

    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    std::cout << "receive body is " << body_str << std::endl;
    connection->_response.set(http::field::content_type, "text/json");
    json body_json;
    json response_json;
    try{
        body_json = json::parse(body_str);
    }catch(json::parse_error& e){
        LOG_ERROR( "Failed to parse JSON data!");
        response_json["error"] = ERRORCODE::Error_Json;
        std::string jsonstr = response_json.dump();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    
    std::string email = body_json["email"];
    std::string name = body_json["user"];
    std::string pwd = body_json["passwd"];
    std::string confirm = body_json["confirm"];
    std::string code = body_json["code"];
    if (pwd != confirm) {
        std::cout << "password err " << std::endl;
        response_json["error"] = ERRORCODE::Password_Not_Equal;
        std::string jsonstr = response_json.dump();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    //先查找redis中email对应的验证码是否合理
    std::string  varify_code;
    std::string key = REDIS_EMAIL_CODE_PREFIX+email;
    bool b_get_varify = RedisMgr::GetInstance()->Get(key, varify_code);
    if (!b_get_varify) {
        std::cout << " get varify code expired" << std::endl;
        response_json["error"] = ERRORCODE::Varify_Code_Expired;
        std::string jsonstr = response_json.dump();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    if (varify_code != code) {
        std::cout << " varify code error" << std::endl;
        response_json["error"] = ERRORCODE::Varify_Code_Not_Equal;
        std::string jsonstr = response_json.dump();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    //查找数据库判断用户是否存在
    int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
    if (uid == 0 || uid == -1) {
        std::cout << " user or email exist" << std::endl;
        response_json["error"] = ERRORCODE::Error_User_Exist;
        std::string jsonstr = response_json.dump();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    }
    response_json["error"] = 0;
    response_json["uid"] = uid;
    response_json["email"] = email;
    response_json ["user"]= name;
    response_json["passwd"] = pwd;
    response_json["confirm"] = confirm;
    response_json["code"] = code;
    std::string jsonstr = response_json.dump();
    beast::ostream(connection->_response.body()) << jsonstr;
    return true;
    });
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con) {
    if (_get_handlers.find(path) == _get_handlers.end()) {
        return false;
    }

    _get_handlers[path](con);
    return true;
}
bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con) {
    if (_post_handlers.find(path) == _post_handlers.end()) {
        return false;
    }

    _post_handlers[path](con);
    return true;
}
LogicSystem::~LogicSystem(){
    
}