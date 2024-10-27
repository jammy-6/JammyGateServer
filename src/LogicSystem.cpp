#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
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

       LOG_INFO( "email is %s" , email.c_str());
       response_json["error_code"] = rsp.error();
       response_json["email"] = rsp.email();
       response_json["code"] = rsp.code();
    
       beast::ostream(connection->_response.body()) << response_json.dump();
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