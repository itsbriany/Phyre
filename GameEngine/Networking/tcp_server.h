#pragma once
#include <boost/asio.hpp>
#include "tcp_server_connection.h"

namespace GameEngine
{
namespace Networking
{
    class TCPServer : public Logging::Loggable
    {

    public:
        TCPServer(boost::asio::io_service& io_service, uint16_t listen_port, const std::queue<std::string>& message_queue = std::queue<std::string>());

        void StartAccept();
        void HandleAccept(const boost::system::error_code& error);

        // Loggable overrides
        std::string log() override { return "[TCPServer]"; }

    private:
        boost::asio::ip::tcp::acceptor m_acceptor;
        TCPServerConnection::pointer m_ptr_connection;
        std::queue<std::string> m_message_queue;
    };
}
}


