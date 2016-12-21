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
        m_acceptor(io_service, tcp::endpoint(tcp::v4(), listen_port)),
        m_message_queue(message_queue) { }

    // For now, this server is only meant to hold one single connection.
    // It will need tweaking to support multiple simultaneous connections.
    void TCPServer::StartAccept() {
        m_ptr_connection = TCPServerConnection::Create(m_acceptor.get_io_service(), m_message_queue);
        if (!m_ptr_connection) {
            Logging::error("No connection available", *this);
            return;
        }

        m_acceptor.async_accept(m_ptr_connection->socket(),
                               boost::bind(&TCPServer::HandleAccept,
                                           this,
                                           boost::asio::placeholders::error));
    }

    void TCPServer::HandleAccept(const boost::system::error_code& error) {
        if (!error)
        {
            m_ptr_connection->Write();
            StartAccept();
        }
    }
}
}


