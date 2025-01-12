//
// Created by root on 24-10-29.
//

#ifndef REDISMGR_H
#define REDISMGR_H

#include "RedisConPool.h"

class RedisMgr : public Singleton<RedisMgr>,
				 public std::enable_shared_from_this<RedisMgr> {
	friend class Singleton<RedisMgr>;

  public:
	~RedisMgr();
	// bool Connect(const std::string& host, int port);
	bool get(const std::string &key, std::string &value);
	bool set(const std::string &key, const std::string &value);
	// bool Auth(const std::string &password);
	bool lPush(const std::string &key, const std::string &value);
	bool lPop(const std::string &key, std::string &value);
	bool rPush(const std::string &key, const std::string &value);
	bool rPop(const std::string &key, std::string &value);
	bool hSet(const std::string &key, const std::string &hkey,
			  const std::string &value);
	bool hSet(const char *key, const char *hkey, const char *hvalue,
			  size_t hvaluelen);
	std::string hGet(const std::string &key, const std::string &hkey);
	bool del(const std::string &key);
	bool existsKey(const std::string &key);
	void close();

  private:
	RedisMgr();
	std::unique_ptr<RedisConPool> _conn_pool;
};

#endif // REDISMGR_H
