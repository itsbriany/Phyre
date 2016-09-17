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
        void Connect(const std::string& host, const std::string& service, const std::string& data_to_send);
        void Disconnect() const;
        void OnHostResolved(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it);
        void OnConnect(const boost::system::error_code& ec);
        void OnRead(const boost::system::error_code& ec, size_t bytes_transferred);
        void OnError(const boost::system::error_code& ec);
        void Write(const std::string data);

        bool is_connected() const { return is_connected_; }

    private:
        boost::asio::io_service& io_service_;
        HostResolver& host_resolver_;
        TCPSocket& tcp_socket_;
        bool is_connected_;
        std::string data_to_send_on_connect_;
    };
}
}


