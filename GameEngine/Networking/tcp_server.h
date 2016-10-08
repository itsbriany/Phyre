#pragma once
#include <boost/asio.hpp>
#include "tcp_connection.h"

namespace GameEngine
{
namespace Networking
{
    class TCPServer
    {

    public:
        TCPServer(boost::asio::io_service& io_service, uint16_t listen_port);

        void StartAccept();
        void HandleAccept(TCPConnection::pointer new_connection, const boost::system::error_code& error);

        friend std::ostream& operator<<(std::ostream& os, const TCPServer& server) {
                return os << "[TCPServer] ";
        }

    private:
        boost::asio::ip::tcp::acceptor acceptor_;
    };
}
}


