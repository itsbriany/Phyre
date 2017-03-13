#include <boost/bind/bind.hpp>
#include "tcp_server.h"

namespace Phyre
{
namespace Networking
{

    using boost::asio::ip::tcp;
    using std::queue;
    using std::string;
    
    const string TCPServer::kWho = "[TCPServer]";

    TCPServer::TCPServer(boost::asio::io_service& io_service, uint16_t listen_port, const queue<string>& message_queue):
        acceptor_(io_service, tcp::endpoint(tcp::v4(), listen_port)),
        message_queue_(message_queue) { }

    // For now, this server is only meant to hold one single connection.
    // It will need tweaking to support multiple simultaneous connections.
    void TCPServer::StartAccept() {
        ptr_connection_ = TCPServerConnection::Create(acceptor_.get_io_service(), message_queue_);
        if (!ptr_connection_) {
            PHYRE_LOG(error, kWho) << "No connection available";
            return;
        }

        acceptor_.async_accept(ptr_connection_->socket(),
                               boost::bind(&TCPServer::HandleAccept,
                                           this,
                                           boost::asio::placeholders::error));
    }

    void TCPServer::HandleAccept(const boost::system::error_code& error) {
        if (!error)
        {
            ptr_connection_->Write();
            StartAccept();
        }
    }
}
}


