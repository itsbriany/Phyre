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
        typedef std::function<void(const boost::system::error_code&, size_t)> OnWriteCallback;


        TCPSocket(boost::asio::io_service& io_service);
        ~TCPSocket();

        void Connect(boost::asio::ip::tcp::resolver::iterator it, OnConnectCallback callback);
        void OnConnect(const boost::system::error_code& ec);
        void Close();
        void OnRead(const boost::system::error_code& ec, size_t bytes_transferred);
        void Write(const std::string data, OnWriteCallback on_read_callback);
        void Read(OnReadCallback on_read_callback);

        // This TCP buffer has a window frame of 4kb
        std::array<char, 4096>& buffer()  { return buffer_; }

        friend std::ostream& operator<<(std::ostream& os, const TCPSocket& tcp_socket) {
            return os << "[TCPSocket] ";
        }

    private:
        void AsyncRead();

        boost::asio::ip::tcp::socket socket_;
        std::array<char, 4096> buffer_;
        OnReadCallback on_read_callback_;
        OnWriteCallback on_write_callback_;
        OnConnectCallback on_connect_callback_;
        bool is_open_;
    };
}
}


