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
	spdlog::level::level_enum log_level =
		static_cast<spdlog::level::level_enum>(
			std::stoi(gConfigMgr["LogSystem"]["Level"]));
	std::string log_path = gConfigMgr["LogSystem"]["Path"] + "/" +
						   gConfigMgr["LogSystem"]["Suffix"];
	try {
		// 创建一个输出到控制台的彩色日志接收器
		auto console_sink =
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);
		console_sink->set_pattern(
			"[%Y年%m月%d日 %H:%M:%S] [%n] [-L-] [t:%t] %v");
		// 创建一个输出到文件的日志接收器
		auto file_sink =
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path, true);
		file_sink->set_level(spdlog::level::trace);
		file_sink->set_pattern("[%Y年%m月%d日 %H:%M:%S] [%n] [%l] [t:%t] %v");
		// 创建一个组合的日志记录器，把控制台和文件接收器都加进去
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(console_sink);
		sinks.push_back(file_sink);

		auto logger = std::make_shared<spdlog::logger>(
			"multi_sink", begin(sinks), end(sinks));
		// 设置全局日志记录器
		spdlog::register_logger(logger);
		logger->flush_on(spdlog::level::trace);
		spdlog::set_level(spdlog::level::trace);
		spdlog::set_default_logger(logger);

	} catch (const spdlog::spdlog_ex &ex) {
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}
}

int main() {
	initSpdlog();
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
		if (isalnum((unsigned char)str[i]) || (str[i] == '-') ||
			(str[i] == '_') || (str[i] == '.') || (str[i] == '~'))
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
