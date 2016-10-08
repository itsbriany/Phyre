#include <boost/bind/bind.hpp>
#include "tcp_server.h"

namespace GameEngine
{
namespace Networking
{

    using boost::asio::ip::tcp;

    TCPServer::TCPServer(boost::asio::io_service& io_service, uint16_t listen_port):
        acceptor_(io_service, tcp::endpoint(tcp::v4(), listen_port)) { }

    void TCPServer::StartAccept() {
        TCPServerConnection::pointer new_connection = TCPServerConnection::create(acceptor_.get_io_service());

        acceptor_.async_accept(new_connection->socket(),
                               boost::bind(&TCPServer::HandleAccept,
                                           this,
                                           new_connection,
                                           boost::asio::placeholders::error));
    }

    void TCPServer::HandleAccept(TCPServerConnection::pointer new_connection,
                                 const boost::system::error_code& error) {
        if (!error)
        {
            new_connection->start();
            StartAccept();
        }
    }
}
}


