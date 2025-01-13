#include "AsioIOServicePool.h"
#include <iostream>
using namespace std;
AsioIOServicePool::AsioIOServicePool(std::size_t size)
	: _ioServices(size), _works(size), _nextIOService(0) {
	for (std::size_t i = 0; i < size; ++i) {
		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));
	}

	// 遍历多个ioservice，创建多个线程，每个线程内部启动ioservice
	for (std::size_t i = 0; i < _ioServices.size(); ++i) {
		_threads.emplace_back([this, i]() { _ioServices[i].run(); });
	}
	spdlog::info("AsioIOServicePool初始化成功");
}

AsioIOServicePool::~AsioIOServicePool() {
	Stop();
	spdlog::info("AsioIOServicePool销毁成功");
}

boost::asio::io_context &AsioIOServicePool::GetIOService() {
	auto &service = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop() {
	// 因为仅仅执行work.reset并不能让iocontext从run的状态中退出
	// 当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务。
	spdlog::info("AsioIOServicePool开始暂停服务");
	for (auto &work : _works) {
		// 把服务先停止
		work->get_io_context().stop();
		work.reset();
	}
	spdlog::info("AsioIOServicePool暂停服务成功");
	for (auto &t : _threads) {
		t.join();
	}
}