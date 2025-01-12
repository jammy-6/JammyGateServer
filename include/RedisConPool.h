#ifndef REDIS_CON_POOL_H
#define REDIS_CON_POOL_H
#include "ConfigMgr.h"
#include "Singleton.h"
#include <atomic>
#include <condition_variable>
#include <hiredis.h>
class RedisConPool {
  public:
	RedisConPool(size_t poolSize, const char *host, int port, const char *pwd);
	~RedisConPool();
	redisContext *getConnection();
	void returnConnection(redisContext *context);
	void close();

  private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	const char *host_;
	int port_;
	std::queue<redisContext *> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
};
#endif