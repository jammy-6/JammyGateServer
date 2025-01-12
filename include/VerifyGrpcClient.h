#ifndef VERIFYGRPCCLIENT_H
#define VERIFYGRPCCLIENT_H

#include "ConfigMgr.h"
#include "Global.h"
#include "RPConPool.h"
#include "Singleton.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <grpcpp/grpcpp.h>
using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
	friend class Singleton<VerifyGrpcClient>;

  public:
	GetVarifyRsp GetVarifyCode(std::string email);

  private:
	VerifyGrpcClient();

	std::unique_ptr<GrpcConnectionPool<VarifyService, VarifyService::Stub>>
		pool_;
};

#endif