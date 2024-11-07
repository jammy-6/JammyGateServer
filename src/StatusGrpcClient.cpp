#include "StatusGrpcClient.h"


GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{

    ClientContext context;
    GetChatServerRsp reply;
    GetChatServerReq request;
    request.set_uid(uid);
    auto stub = pool_->getConnection();
    Status status = stub->GetChatServer(&context, request, &reply);
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
StatusGrpcClient::StatusGrpcClient()
{
    
    std::string host = gConfigMgr["StatusServer"]["Host"];
    std::string port = gConfigMgr["StatusServer"]["Port"];
    pool_.reset(new StatusConPool(5, host, port));
    LOG_INFO("StatusGrpcClient初始化成功");
}