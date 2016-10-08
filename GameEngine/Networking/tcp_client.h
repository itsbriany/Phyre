#pragma once
#include <boost/asio.hpp>
#include "host_resolver.h"
#include "tcp_socket.h"

namespace GameEngine
{
namespace Networking
{
    class TCPClient
    {

    public:
        TCPClient(std::unique_ptr<HostResolver> resolver, std::unique_ptr<TCPSocket> tcp_socket);
        ~TCPClient();

        static std::unique_ptr<TCPClient> MakeTCPClient(boost::asio::io_service& io_service, const std::string& host, const std::string& service);

        void Connect(const std::string& host, const std::string& service, const std::string& data_to_send);
        void Disconnect() const;
        void OnHostResolved(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it);
        void OnConnect(const boost::system::error_code& ec);
        void OnRead(const boost::system::error_code& ec, size_t bytes_transferred);
        void OnError(const boost::system::error_code& ec);
        void Write(const std::string& data);

        bool is_connected() const { return is_connected_; }

        friend std::ostream& operator<<(std::ostream& os, const TCPClient& cc) {
                return os << "[TCPClient] ";
        }

    private:
        std::unique_ptr<HostResolver> host_resolver_;
        std::unique_ptr<TCPSocket> tcp_socket_;
        bool is_connected_;
        std::string data_to_send_on_connect_;
    };
}
}


