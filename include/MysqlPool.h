
#ifndef MYSQLPOOL_H
#define MYSQLPOOL_H
#include "Global.h"
#include <condition_variable>
#include <mutex>
#include <mysql_connection.h>
#include <mysql_driver.h>
class MysqlPool {
  public:
	MysqlPool(const std::string &url, const std::string &user,
			  const std::string &pass, const std::string &schema, int poolSize);
	std::unique_ptr<sql::Connection> getConnection();
	void returnConnection(std::unique_ptr<sql::Connection> con);
	void close();
	~MysqlPool();

  private:
	std::string url_;
	std::string user_;
	std::string pass_;
	std::string schema_;
	int poolSize_;
	std::queue<std::unique_ptr<sql::Connection>> pool_;
	std::mutex mutex_;
	std::condition_variable cond_;
	std::atomic<bool> b_stop_;
};

#endif // MYSQLPOOL_H
