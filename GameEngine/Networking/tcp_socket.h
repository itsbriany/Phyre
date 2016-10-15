#pragma once
#include <boost/asio.hpp>
#include <queue>

namespace GameEngine
{
namespace Networking
{
    class TCPSocket
    {
    public:

        typedef std::function<void(const boost::system::error_code&)> OnConnectCallback;
        typedef std::function<void(const boost::system::error_code&, size_t)> OnReadCallback;
        typedef std::function<void(const boost::system::error_code&, size_t)> OnWriteCallback;

		
        TCPSocket(boost::asio::io_service& io_service,
        		  OnConnectCallback on_connect_callback,
        		  OnReadCallback on_read_callback,
        		  OnWriteCallback on_write_callback);
        ~TCPSocket();

        void Connect(boost::asio::ip::tcp::resolver::iterator it);
        void Close();
        void Write(const std::string& data);
        void Read();
		void ResetMessageQueue(std::queue<std::string> new_message_queue = std::queue<std::string>());

        // This TCP buffer has a window frame of 4kb
        std::array<char, 4096>& buffer()  { return buffer_; }
		std::queue<std::string> message_queue() const { return message_queue_; }
		bool is_connected() const { return is_connected_; }

        friend std::ostream& operator<<(std::ostream& os, const TCPSocket& tcp_socket) {
            return os << "[TCPSocket] ";
        }

    private:
		void OnConnect(const boost::system::error_code& ec);
		void OnRead(const boost::system::error_code& ec, size_t bytes_transferred);

        boost::asio::ip::tcp::socket socket_;
        std::array<char, 4096> buffer_;
		std::queue<std::string> message_queue_;
		OnConnectCallback on_connect_callback_;
        OnReadCallback on_read_callback_;
		OnWriteCallback on_write_callback_;
        bool is_connected_;
    };
}
}


