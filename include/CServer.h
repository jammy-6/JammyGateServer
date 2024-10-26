#ifndef CSERVER_H
#define CSERVER_H

#include "Global.h"

class CServer:public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    void Start();
private:
    tcp::acceptor  _acceptor;
    net::io_context& _ioc;
    boost::asio::ip::tcp::socket   _socket;
};

#endif