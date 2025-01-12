
#include "MysqlPool.h"

MysqlPool::MysqlPool(const std::string &url, const std::string &user,
					 const std::string &pass, const std::string &schema,
					 int poolSize)
	: url_(url), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize),
	  b_stop_(false) {
	try {
		for (int i = 0; i < poolSize_; ++i) {
			sql::mysql::MySQL_Driver *driver =
				sql::mysql::get_mysql_driver_instance();
			std::unique_ptr<sql::Connection> con(
				driver->connect(url_, user_, pass_));
			con->setSchema(schema_);
			pool_.push(std::move(con));
		}
	} catch (sql::SQLException &e) {
		// 处理异常
		spdlog::error("MySQL连接池初始化异常");
	}
}
std::unique_ptr<sql::Connection> MysqlPool::getConnection() {
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this] {
		if (b_stop_) {
			return true;
		}
		return !pool_.empty();
	});
	if (b_stop_) {
		return nullptr;
	}
	std::unique_ptr<sql::Connection> con(std::move(pool_.front()));
	pool_.pop();
	return con;
}
void MysqlPool::returnConnection(std::unique_ptr<sql::Connection> con) {
	std::unique_lock<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	pool_.push(std::move(con));
	cond_.notify_one();
}
void MysqlPool::close() {
	b_stop_ = true;
	cond_.notify_all();
}
MysqlPool::~MysqlPool() {
	std::unique_lock<std::mutex> lock(mutex_);
	while (!pool_.empty()) {
		pool_.pop();
	}
}
