#pragma once
#include <boost/asio.hpp>

namespace GameEngine
{
namespace Networking
{
    class TCPSocket
    {
    public:
        typedef std::function<void(const boost::system::error_code&)> OnConnectCallback;

        // Arg1: Error on read
        // Arg2: Bytes read
        typedef std::function<void(const boost::system::error_code&, size_t)> OnReadCallback;

        virtual ~TCPSocket() { }

        // Triggers the callback function once a connection to the remote endpoint has been established
        virtual void Connect(boost::asio::ip::tcp::resolver::iterator it, OnConnectCallback callback) = 0;
        virtual void OnRead(const boost::system::error_code& ec, size_t bytes_transferred) = 0;
        virtual void Write(const std::string data, OnReadCallback on_read_callback) = 0;
        virtual void Read() = 0;
        
        virtual std::array<char, 4096>& buffer() = 0;
        
        // Gracefully close the connection
        virtual void Close() = 0;
    };
}
}