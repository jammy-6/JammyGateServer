#include "LogicSystem.h"
#include "HttpConnection.h"
#include "MysqlMgr.h"
#include "RedisMgr.h"
#include "StatusGrpcClient.h"
#include "VerifyGrpcClient.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

void LogicSystem::regGet(std::string url, HttpHandler handler) {
	getHandlers_.insert(make_pair(url, handler));
}

void LogicSystem::regPost(std::string url, HttpHandler handler) {
	postHandlers_.insert(make_pair(url, handler));
}
LogicSystem::LogicSystem() {

	regGet("/get_test",
		   std::bind(&LogicSystem::handleGetTest, this, std::placeholders::_1));
	//
	regPost("/get_varifycode_register",
			std::bind(&LogicSystem::handleGetVarifyCodeRegister, this,
					  std::placeholders::_1));

	regPost("/get_varifycode_reset",
			std::bind(&LogicSystem::handleGetVarifyCodeReset, this,
					  std::placeholders::_1));

	regPost("/user_register", std::bind(&LogicSystem::handleUserRegister, this,
										std::placeholders::_1));
	regPost("/user_reset_password",
			std::bind(&LogicSystem::handleUserResetPassword, this,
					  std::placeholders::_1));

	regPost("/user_login", std::bind(&LogicSystem::handleUserLogin, this,
									 std::placeholders::_1));
}

bool LogicSystem::handleGet(std::string path,
							std::shared_ptr<HttpConnection> con) {
	if (getHandlers_.find(path) == getHandlers_.end()) {
		return false;
	}

	getHandlers_[path](con);
	return true;
}
bool LogicSystem::handlePost(std::string path,
							 std::shared_ptr<HttpConnection> con) {
	if (postHandlers_.find(path) == postHandlers_.end()) {
		return false;
	}

	postHandlers_[path](con);
	return true;
}
bool LogicSystem::handleGetTest(std::shared_ptr<HttpConnection> connection) {
	spdlog::info("get_test : 连接测试成功");
	beast::ostream(connection->_response.body()) << "receive get_test req";
	return true;
}
bool LogicSystem::handleGetVarifyCodeRegister(
	std::shared_ptr<HttpConnection> connection) {
	spdlog::info("get_varifycode_register 服务调用开始");
	auto body_str =
		boost::beast::buffers_to_string(connection->_request.body().data());
	spdlog::info("接收到的HTTP请求体为{} ", body_str.c_str());
	connection->_response.set(http::field::content_type, "text/json");

	json body_json;
	json response_json;
	try {
		body_json = json::parse(body_str);
	} catch (json::parse_error &e) {
		spdlog::error("json解析失败，获取验证码失败，即将返回");
		response_json["error_code"] = ERRORCODE::Error_Json;
		response_json["body"] = body_str;
		beast::ostream(connection->_response.body()) << response_json;
		return true;
	}
	std::string email = body_json["email"];
	std::string name = body_json["user"];
	/// 要重置密码的用户不存在
	if (!MysqlMgr::GetInstance()->checkUserExist(name, email)) {
		spdlog::error("用户名或邮箱已经存在，获取验证码失败，即将返回");
		response_json["error_code"] = ERRORCODE::Error_User_Exist;
		response_json["body"] = body_str;
		beast::ostream(connection->_response.body()) << response_json;
		return true;
	}
	GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);

	auto err = rsp.error();
	/// 有问题
	spdlog::info("VerifyGrpcClient服务调用成功");
	response_json["error_code"] = 0;
	response_json["email"] = rsp.email();
	/// 验证码是没必要返回给客户端的，这里只是方便调试以及新增用户
	response_json["code"] = rsp.code();
	beast::ostream(connection->_response.body()) << response_json.dump();
	spdlog::info("用户{}，邮箱为{},获取验证码{}成功", name.c_str(),
				 email.c_str(), rsp.code().c_str());
	return true;
}

bool LogicSystem::handleGetVarifyCodeReset(
	std::shared_ptr<HttpConnection> connection) {
	std::string body_str =
		boost::beast::buffers_to_string(connection->_request.body().data());
	spdlog::info("用户重置密码获取验证码请求，body为{}", body_str.c_str());
	connection->_response.set(http::field::content_type, "text/json");

	json body_json;
	json response_json;
	try {
		body_json = json::parse(body_str);
	} catch (json::parse_error &e) {
		spdlog::info("用户重置密码获取验证码请求，JSON解析失败");
		response_json["error_code"] = ERRORCODE::Error_Json;
		response_json["body"] = body_str;
		beast::ostream(connection->_response.body()) << response_json;
		return true;
	}
	std::string email = body_json["email"];
	std::string user = body_json["user"];
	/// 查询用户名与邮箱是否匹配
	if (!MysqlMgr::GetInstance()->checkUserEmailMatch(user, email)) {
		/// 用户名邮箱不匹配
		spdlog::info("用户重置密码获取验证码请求，用户名与邮箱不匹配");
		response_json["error_code"] = ERRORCODE::ERROR_USER_EMAIL_NOT_MATCH;
		beast::ostream(connection->_response.body()) << response_json.dump();
		return true;
	}

	GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
	spdlog::info(
		"用户重置密码获取验证码请求，验证码发送服务调用返回，响应码为%d",
		rsp.error());
	auto err = rsp.error();
	response_json["error_code"] = 0;
	response_json["email"] = rsp.email();
	/// 验证码是没必要返回给客户端的，这里只是方便调试以及新增用户
	response_json["code"] = rsp.code();
	// std::string key = REDIS_EMAIL_CODE_PREFIX + email;
	// RedisMgr::GetInstance()->Set(key, rsp.code());
	beast::ostream(connection->_response.body()) << response_json.dump();
	return true;
}
bool LogicSystem::handleUserRegister(
	std::shared_ptr<HttpConnection> connection) {
	auto body_str =
		boost::beast::buffers_to_string(connection->_request.body().data());
	spdlog::info("收到用户注册请求，body为{}", body_str.c_str());
	connection->_response.set(http::field::content_type, "text/json");
	json body_json;
	json response_json;
	try {
		body_json = json::parse(body_str);
	} catch (json::parse_error &e) {
		spdlog::error("用户注册请求解析失败！");
		response_json["error_code"] = ERRORCODE::Error_Json;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
	std::string email = body_json["email"];
	std::string name = body_json["user"];
	std::string pwd = body_json["passwd"];
	std::string code = body_json["code"];
	// 先查找redis中email对应的验证码是否合理
	std::string varify_code;
	std::string key = REDIS_EMAIL_CODE_PREFIX + email;
	bool b_get_varify = RedisMgr::GetInstance()->get(key, varify_code);
	if (!b_get_varify) {
		spdlog::info("{},{}对应的验证码已过期！", name.c_str(), email.c_str());
		response_json["error_code"] = ERRORCODE::Varify_Code_Expired;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
	if (varify_code != code) {
		spdlog::info("{},{}注册验证码不一致！", name.c_str(), email.c_str());
		response_json["error_code"] = ERRORCODE::Varify_Code_Not_Equal;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}

	int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
	if (uid == 0 || uid == -1) {
		spdlog::info("{},{}用户或邮箱已存在！", name.c_str(), email.c_str());
		response_json["error_code"] = ERRORCODE::Error_User_Exist;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
	spdlog::info("{},{}用户注册成功！", name.c_str(), email.c_str());
	response_json["error_code"] = ERRORCODE::Success;
	std::string jsonstr = response_json.dump();
	beast::ostream(connection->_response.body()) << jsonstr;
	return true;
}
bool LogicSystem::handleUserResetPassword(
	std::shared_ptr<HttpConnection> connection) {
	auto body_str =
		boost::beast::buffers_to_string(connection->_request.body().data());
	spdlog::info("收到用户重置密码请求，body为{}", body_str.c_str());
	connection->_response.set(http::field::content_type, "text/json");
	json body_json;
	json response_json;
	try {
		body_json = json::parse(body_str);
	} catch (json::parse_error &e) {
		spdlog::error("用户重置密码请求JSON解析失败！");
		response_json["error_code"] = ERRORCODE::Error_Json;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
	std::string email = body_json["email"];
	std::string name = body_json["user"];
	std::string pwd = body_json["passwd"];
	std::string code = body_json["code"];

	// 先查找redis中email对应的验证码是否合理
	std::string varify_code;
	std::string key = REDIS_EMAIL_CODE_PREFIX + email;
	bool b_get_varify = RedisMgr::GetInstance()->get(key, varify_code);
	if (!b_get_varify) {
		spdlog::error("用户重置密码，验证码已过期！");
		response_json["error_code"] = ERRORCODE::Varify_Code_Expired;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
	if (varify_code != code) {
		spdlog::error(
			"用户重置密码，验证码错误！，正确验证码为{}，用户的验证码为{}",
			varify_code.c_str(), code.c_str());
		response_json["error_code"] = ERRORCODE::Varify_Code_Not_Equal;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
	// 查找数据库判断用户是否存在
	if (MysqlMgr::GetInstance()->checkUserExist(name, email)) {
		spdlog::error("用户重置密码，用户不存在！");
		response_json["error_code"] = ERRORCODE::Error_User_Not_Exist;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}

	if (MysqlMgr::GetInstance()->updatePassword(name, email, pwd)) {
		response_json["error_code"] = ERRORCODE::Success;
		response_json["email"] = email;
		response_json["user"] = name;
		response_json["passwd"] = pwd;
		response_json["code"] = code;
		spdlog::error("用户重置密码成功！");
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	} else {
		spdlog::error(
			"用户重置密码，用户名或邮箱不存在，或者密码与重置密码一致！");
		response_json["error_code"] = ERRORCODE::Error_Update_Password;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
}
bool LogicSystem::handleUserLogin(std::shared_ptr<HttpConnection> connection) {
	auto body_str =
		boost::beast::buffers_to_string(connection->_request.body().data());
	spdlog::info("收到用户登录请求，body为{}", body_str.c_str());
	connection->_response.set(http::field::content_type, "text/json");
	json request_json;
	json response_json;
	try {
		request_json = json::parse(body_str);
	} catch (json::parse_error &e) {
		spdlog::info("用户登录请求，JSON解析失败！");
		response_json["error_code"] = ERRORCODE::Error_Json;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}

	std::string name = request_json["user"];
	std::string pwd = request_json["passwd"];
	/// UserInfo userInfo;
	// 查询数据库判断用户名和密码是否匹配
	UserInfo userInfo;
	bool pwd_valid =
		MysqlMgr::GetInstance()->checkUserPassword(name, pwd, userInfo);
	if (!pwd_valid) {
		spdlog::info("用户登录请求，用户名{}，密码{}不匹配！", name.c_str(),
					 pwd.c_str());
		response_json["error_code"] = ERRORCODE::ERROR_PASSWORD_NOT_CORRECT;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}
	spdlog::info("用户登录请求，用户名{}，密码{}匹配，正在获取聊天服务器信息",
				 name.c_str(), pwd.c_str());
	// 查询StatusServer找到合适的连接
	auto reply = StatusGrpcClient::GetInstance()->GetChatServer(userInfo.uid);

	if (reply.error()) {
		spdlog::error("rpc调用失败,错误是%d", reply.error());
		response_json["error_code"] = ERRORCODE::RPCFailed;
		std::string jsonstr = response_json.dump();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
	}

	spdlog::info("获取聊天服务器信息成功，grpc::GetChatServer服务,uid为%d",
				 userInfo.uid);
	response_json["error_code"] = 0;
	response_json["user"] = name;
	response_json["uid"] = std::to_string(userInfo.uid);
	response_json["port"] = reply.port();
	response_json["token"] = reply.token();
	response_json["host"] = reply.host();
	std::string jsonstr = response_json.dump();
	spdlog::info("获取聊天服务器信息成功，返回聊天服务器响应包体{}",
				 jsonstr.c_str());
	beast::ostream(connection->_response.body()) << jsonstr;

	return true;
}
LogicSystem::~LogicSystem() {}