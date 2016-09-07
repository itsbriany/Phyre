#pragma once
#include <boost/asio.hpp>
#include "host_resolver.h"
#include "tcp_socket.h"

namespace GameEngine
{
namespace Networking
{
    class ChatClient
    {
        
    public:
        ChatClient(boost::asio::io_service& io_service, HostResolver& resolver, TCPSocket& tcp_socket);
        ~ChatClient();
        void Connect(const std::string& host, const std::string& service);
        void OnHostResolved(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it);
        void OnConnect(const boost::system::error_code& ec);
        void OnRead(const boost::system::error_code& ec, const std::string& data);
        void OnError(const boost::system::error_code& ec);
        void Write(const std::string data) const;

    private:
        boost::asio::io_service& io_service_;
        HostResolver& host_resolver_;
        TCPSocket& tcp_socket_;
        bool is_connected_;
    };
}
}


