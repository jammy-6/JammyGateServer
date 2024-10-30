#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "Global.h"
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class HttpConnection: public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;
public:

    explicit HttpConnection(boost::asio::io_context& context);
    // HttpConnection(tcp::socket socket);
    void Start();
    tcp::socket& GetSocket();
private:
    void CheckDeadline();
    void WriteResponse();
    void HandleReq();
    tcp::socket  _socket;
    boost::asio::io_context& _context;
    // The buffer for performing reads.
    beast::flat_buffer  _buffer{ 8192 };

    // The request message.
    http::request<http::dynamic_body> _request;

    // The response message.
    http::response<http::dynamic_body> _response;

    // The timer for putting a deadline on connection processing.
    net::steady_timer deadline_{
        _socket.get_executor(), std::chrono::seconds(60) };
};

#endif