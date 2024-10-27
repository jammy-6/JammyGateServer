#ifndef GLOBAL_H
#define GLOBAL_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "nlohmann/json.hpp"
#include "log.h"
#include "Singleton.h"
#include "Config.h"
#include "ConfigMgr.h"
#include <string>
#include <map>
enum ERRORCODE {
    Success = 0,
    Error_Json = 1001,  //Json解析错误
    RPCFailed = 1002,  //RPC请求错误
};
class ConfigMgr;
extern ConfigMgr gConfigMgr;
using json = nlohmann::json;
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#endif