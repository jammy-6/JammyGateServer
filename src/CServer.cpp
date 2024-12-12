#include "CServer.h"
#include "AsioIOServicePool.h"
#include "HttpConnection.h"

CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
    : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
    LOG_INFO("CServer初始化成功，端口为%d",
             (int)_acceptor.local_endpoint().port());
}

void CServer::Start() {
    auto self = shared_from_this();
    std::shared_ptr<HttpConnection> newConn = std::make_shared<HttpConnection>(
        AsioIOServicePool::GetInstance()->GetIOService());
    _acceptor.async_accept(
        newConn->GetSocket(), [self, newConn](beast::error_code ec) {
            try {
                // 出错则放弃这个连接，继续监听新链接
                if (ec) {
                    self->Start();
                    return;
                }
                tcp::endpoint remote_endpoint =
                    newConn->GetSocket().remote_endpoint();
                // 处理新链接，创建HpptConnection类管理新连接
                LOG_INFO("接收到新连接：IP为%s,端口为%d",
                         remote_endpoint.address().to_string().c_str(),
                         (int)remote_endpoint.port());
                newConn->Start();
                // 继续监听
                self->Start();
            } catch (std::exception &exp) {

                LOG_ERROR("CServer发生异常：%s", exp.what());
                self->Start();
            }
        });
}