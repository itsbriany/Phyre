#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

namespace GameEngine
{
namespace Networking
{
    // Keep the connection object alive as long as an object is referencing it
    class TCPServerConnection : public std::enable_shared_from_this<TCPServerConnection>
    {

    public:
        typedef std::shared_ptr<TCPServerConnection> pointer;

        static pointer create(boost::asio::io_service& io_service) {
            return pointer(new TCPServerConnection(io_service));
        }

        boost::asio::ip::tcp::socket& socket() {
            return socket_;
        }

        void start() {
            message_ = "Greetings From TCPServerConnection!\r\n";

            boost::asio::async_write(socket_, boost::asio::buffer(message_),
                    boost::bind(&TCPServerConnection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }

        friend std::ostream& operator<<(std::ostream& os, const TCPServerConnection& connection) {
            return os << "[TCPServerConnection] ";
        }

    private:
        TCPServerConnection(boost::asio::io_service& io_service): socket_(io_service) {}

		void handle_write(const boost::system::error_code& /*error*/,
						  size_t /*bytes_transferred*/) { }

        boost::asio::ip::tcp::socket socket_;
        std::string message_;
    };
}
}


