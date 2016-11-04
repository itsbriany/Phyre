#include <boost/bind/bind.hpp>
#include "tcp_server.h"

namespace GameEngine
{
namespace Networking
{

    using boost::asio::ip::tcp;
    using std::queue;
    using std::string;

    TCPServer::TCPServer(boost::asio::io_service& io_service, uint16_t listen_port, const queue<string>& message_queue):
        acceptor_(io_service, tcp::endpoint(tcp::v4(), listen_port)),
        message_queue_(message_queue) { }

    // For now, this server is only meant to hold one single connection.
    // It will need tweaking to support multiple simultaneous connections.
    void TCPServer::StartAccept() {
        p_connection_ = TCPServerConnection::Create(acceptor_.get_io_service(), message_queue_);
        if (!p_connection_) {
            Logging::error("No connection available", *this);
            return;
        }

        acceptor_.async_accept(p_connection_->socket(),
                               boost::bind(&TCPServer::HandleAccept,
                                           this,
                                           boost::asio::placeholders::error));
    }

    void TCPServer::HandleAccept(const boost::system::error_code& error) {
        if (!error)
        {
            p_connection_->Write();
            StartAccept();
        }
    }
}
}


