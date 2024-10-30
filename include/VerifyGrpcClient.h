#ifndef VERIFYGRPCCLIENT_H
#define VERIFYGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include "Global.h"
#include "Singleton.h"
#include "RPConPool.h"
#include "ConfigMgr.h"
class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:

    GetVarifyRsp GetVarifyCode(std::string email) {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;
        request.set_email(email);
        auto stub = pool_->getConnection();
        Status status = stub->GetVarifyCode(&context, request, &reply);

        if (status.ok()) {

            pool_->returnConnection(std::move(stub));
            return reply;
        }
        else {

            reply.set_error(ERRORCODE::RPCFailed);
            pool_->returnConnection(std::move(stub));
            return reply;
        }
    }

private:
    VerifyGrpcClient(){
        
        std::string ip = gConfigMgr["VarifyServer"]["Ip"];
        std::string port = gConfigMgr["VarifyServer"]["Port"];
        std::string size = gConfigMgr["VarifyServer"]["RPCChannelSize"];
        pool_.reset(new RPConPool(stoi(size),ip,port));
        // std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
        // stub_ = VarifyService::NewStub(channel);
    }

    std::unique_ptr<RPConPool> pool_;
    // std::unique_ptr<VarifyService::Stub> stub_;
};

#endif