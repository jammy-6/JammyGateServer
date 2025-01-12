#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "Global.h"
#include "Singleton.h"
#include <functional>
#include <map>

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
class LogicSystem : public Singleton<LogicSystem> {
	friend class Singleton<LogicSystem>;

  public:
	~LogicSystem();
	bool handleGet(std::string, std::shared_ptr<HttpConnection>);
	bool handlePost(std::string path, std::shared_ptr<HttpConnection> con);
	void regGet(std::string, HttpHandler handler);
	void regPost(std::string url, HttpHandler handler);

	// 
	bool handleGetTest(std::shared_ptr<HttpConnection> connection);

	bool
	handleGetVarifyCodeRegister(std::shared_ptr<HttpConnection> connection);
	bool handleGetVarifyCodeReset(std::shared_ptr<HttpConnection> connection);
    bool handleUserRegister(std::shared_ptr<HttpConnection> connection);
    bool handleUserResetPassword(std::shared_ptr<HttpConnection> connection);
    bool handleUserLogin(std::shared_ptr<HttpConnection> connection);
  private:
	LogicSystem();
	std::map<std::string, HttpHandler> postHandlers_;
	std::map<std::string, HttpHandler> getHandlers_;
};

#endif