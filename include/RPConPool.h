#ifndef RPCONPOOL_H
#define RPCONPOOL_H
#include <condition_variable>
#include <functional>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

// grpc相关
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

template <class StubService, class StubType> class GrpcConnectionPool {
  public:
    // 构造函数，传入目标服务器地址
    GrpcConnectionPool(const std::string &target, size_t poolSize)
        : server_target(target), poolSize_(poolSize) {
        // 初始化最小连接数
        for (int i = 0; i < poolSize_; ++i) {
            addConnection();
        }
    }

    ~GrpcConnectionPool() {
        std::unique_lock<std::mutex> lock(mutex_);
        close();
        while (!connections.empty()) {
            auto conn = connections.front();
            connections.pop();
            conn.reset();
        }
    }

    void close() {
        b_stop_ = true;
        cv.notify_all();
    }

    // 获取一个连接
    std::shared_ptr<StubType> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        // 如果连接池为空，等待直到有可用连接
        while (connections.empty()) {
            cv.wait(lock);
        }
        // 如果停止则直接返回空指针
        if (b_stop_) {
            return nullptr;
        }
        auto conn = connections.front();
        connections.pop();
        return conn;
    }

    // 归还连接
    void returnConnection(std::shared_ptr<StubType> conn) {
        std::unique_lock<std::mutex> lock(mutex_);
        connections.push(conn);
        cv.notify_one();
    }

  private:
    // 添加一个新连接
    void addConnection() {
        auto channel = grpc::CreateChannel(server_target,
                                           grpc::InsecureChannelCredentials());
        std::unique_lock<std::mutex> lock(mutex_);
        connections.push(StubService::NewStub(channel));
    }

    const std::string server_target;
    std::atomic<bool> b_stop_;
    size_t poolSize_;
    std::queue<std::shared_ptr<StubType>> connections;
    std::mutex mutex_;
    std::condition_variable cv;
};

#endif