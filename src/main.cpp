#include "CServer.h"
#include "ConfigMgr.h"
#include "GlobalTool.h"

int main() {
	// 初始化日志库配置
	initSpdlog();
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
