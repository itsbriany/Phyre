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
        void Close() override;
        void OnRead(const boost::system::error_code& ec, size_t bytes_transferred) override;
        void Write(const std::string data, OnReadCallback on_read_callback) override;
        void Read() override;

        const std::vector<char>& buffer() const override { return buffer_; }


    private:
        void AsyncRead();

        boost::asio::ip::tcp::socket socket_;
        std::vector<char> buffer_;
        OnReadCallback on_read_callback_;
    };
}
}


