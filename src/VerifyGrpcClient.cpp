#include "VerifyGrpcClient.h"

GetVarifyRsp VerifyGrpcClient::GetVarifyCode(std::string email) {
    ClientContext context;
    GetVarifyRsp reply;
    GetVarifyReq request;
    request.set_email(email);
    auto stub = pool_->getConnection();
    Status status = stub->GetVarifyCode(&context, request, &reply);

    if (status.ok()) {

        pool_->returnConnection(std::move(stub));
        return reply;
    } else {

        reply.set_error(ERRORCODE::RPCFailed);
        pool_->returnConnection(std::move(stub));
        return reply;
    }
}

VerifyGrpcClient::VerifyGrpcClient() {

    std::string ip = gConfigMgr["VarifyServer"]["Ip"];
    std::string port = gConfigMgr["VarifyServer"]["Port"];
    std::string size = gConfigMgr["VarifyServer"]["RPCChannelSize"];
    pool_.reset(new GrpcConnectionPool<VarifyService, VarifyService::Stub>(
        ip + ":" + port, stoi(size)));
    // std::shared_ptr<Channel> channel =
    // grpc::CreateChannel("127.0.0.1:50051",
    // grpc::InsecureChannelCredentials()); stub_ =
    // VarifyService::NewStub(channel);
}