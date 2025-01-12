#ifndef STATUSGRPCCLIENT_H
#define STATUSGRPCCLIENT_H
#include "ConfigMgr.h"
#include "Global.h"
#include "Singleton.h"
#include "proto/message.grpc.pb.h"
#include "proto/message.pb.h"
#include <RPConPool.h>
#include <atomic>
#include <condition_variable>
#include <grpcpp/grpcpp.h>
// grpc相关
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusGrpcClient : public Singleton<StatusGrpcClient> {
	friend class Singleton<StatusGrpcClient>;

  public:
	~StatusGrpcClient() {}
	GetChatServerRsp GetChatServer(int uid);

  private:
	StatusGrpcClient();
	std::unique_ptr<GrpcConnectionPool<StatusService, StatusService::Stub>>
		pool_;
};

#endif