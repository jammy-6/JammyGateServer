#include "CServer.h"
#include "Global.h"
#include "HttpConnection.h"
#include "LogicSystem.h"

#include "ConfigMgr.h"
#include "MysqlMgr.h"
#include "RedisMgr.h"
#include "StatusGrpcClient.h"
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>

void initSpdlog() {
  // 控制台logger
  spdlog::level::level_enum log_level = static_cast<spdlog::level::level_enum>(
      std::stoi(gConfigMgr["LogSystem"]["Level"]));
  std::string log_path =
      gConfigMgr["LogSystem"]["Path"] + "/" + gConfigMgr["LogSystem"]["Suffix"];
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  //  文件logger
  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path, true);
  // 复合logger
  std::vector<spdlog::sink_ptr> sinks = {console_sink, file_sink};

  std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(
      "GateServer", sinks.begin(), sinks.end());
  logger->set_level(log_level);
  logger->set_pattern(
      "[%Y年%m月%d日 %H:%M:%S %z] [%n] [%^-%L-%$] [thread %t] %v");
  spdlog::set_default_logger(logger);
}

int main() {

  MysqlMgr::GetInstance();
  RedisMgr::GetInstance();
  StatusGrpcClient::GetInstance();
  try {
    std::string portString = gConfigMgr["GateServer"]["Port"];
    unsigned short port = static_cast<unsigned short>(
        std::stoi(gConfigMgr["GateServer"]["Port"]));
    net::io_context ioc{1};
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](const boost::system::error_code &error, int signal_number) {
          if (error) {
            return 0;
          }
          ioc.stop();
          return 0;
        });
    std::make_shared<CServer>(ioc, port)->Start();
    ioc.run();
    return 0;
  } catch (std::exception const &e) {
    spdlog::error("async_wait调用异常：{}}", e.what());
    return EXIT_FAILURE;
  }
}

unsigned char ToHex(unsigned char x) { return x > 9 ? x + 55 : x + 48; }

unsigned char FromHex(unsigned char x) {
  unsigned char y;
  if (x >= 'A' && x <= 'Z')
    y = x - 'A' + 10;
  else if (x >= 'a' && x <= 'z')
    y = x - 'a' + 10;
  else if (x >= '0' && x <= '9')
    y = x - '0';
  else
    assert(0);
  return y;
}
std::string UrlEncode(const std::string &str) {
  std::string strTemp;
  size_t length = str.length();
  for (size_t i = 0; i < length; i++) {
    // 判断是否仅有数字和字母构成
    if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') ||
        (str[i] == '.') || (str[i] == '~'))
      strTemp += str[i];
    else if (str[i] == ' ') // 为空字符
      strTemp += "+";
    else {
      // 其他字符需要提前加%并且高四位和低四位分别转为16进制
      strTemp += '%';
      strTemp += ToHex((unsigned char)str[i] >> 4);
      strTemp += ToHex((unsigned char)str[i] & 0x0F);
    }
  }
  return strTemp;
}
