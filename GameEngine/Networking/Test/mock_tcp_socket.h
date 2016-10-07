#pragma once
#include <gmock/gmock.h>
#include "tcp_socket.h"

namespace GameEngine
{
namespace Networking
{
    class MockTCPSocket : public TCPSocket
    {
    public:
        MOCK_METHOD2(Connect, void(boost::asio::ip::tcp::resolver::iterator it, OnConnectCallback callback));
        MOCK_METHOD2(OnRead, void(const boost::system::error_code& ec, size_t bytes_transferred));
        MOCK_METHOD2(Write, void(const std::string data, OnReadCallback on_read_callback));
        MOCK_METHOD0(Close, void());
        MOCK_METHOD0(Read, void());
        
        std::array<char, 4096>& buffer() override { return buffer_; }

        std::array<char, 4096> buffer_;
    };
}
}
