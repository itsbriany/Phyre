#pragma once
#include "tcp_socket.h"

namespace GameEngine
{
namespace Networking
{
    class TCPSocketImpl : public TCPSocket
    {
    public:
        TCPSocketImpl(boost::asio::io_service& io_service);
        ~TCPSocketImpl();

        void Connect(boost::asio::ip::tcp::resolver::iterator it, OnConnectCallback callback) override;
        void OnConnect(const boost::system::error_code& ec);
        void Close() override;
        void OnRead(const boost::system::error_code& ec, size_t bytes_transferred) override;
        void Write(const std::string data, OnReadCallback on_read_callback) override;
        void Read() override;

        // This TCP buffer has a window frame of 4kb
        std::array<char, 4096>& buffer() override { return buffer_; }

		friend std::ostream& operator<<(std::ostream& os, const TCPSocketImpl& tcp_socket_impl) {
			return os << "[TCPSocketImpl] ";
		}

    private:
        void AsyncRead();

        std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
        std::array<char, 4096> buffer_;
        OnReadCallback on_read_callback_;
        OnConnectCallback on_connect_callback_;
        bool is_open_;
    };
}
}


