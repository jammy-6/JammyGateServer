#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"


CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :
_ioc(ioc),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port)),
_socket(ioc)
{
    
    LOG_INFO("Server port is %d",(int)_acceptor.local_endpoint().port());
}

void CServer::Start()
{    
    auto self = shared_from_this();
    std::shared_ptr<HttpConnection> newConn = std::make_shared<HttpConnection>(AsioIOServicePool::GetInstance()->GetIOService());
    _acceptor.async_accept(newConn->GetSocket(), [self,newConn](beast::error_code ec) {
        try {
            //出错则放弃这个连接，继续监听新链接
            if (ec) {
                self->Start();
                return;
            }
            //处理新链接，创建HpptConnection类管理新连接
            newConn->Start();
            //继续监听
            self->Start();
        }
        catch (std::exception& exp) {
            
            LOG_ERROR("exception is %s" ,  exp.what() );
            self->Start();
        }
    });
}