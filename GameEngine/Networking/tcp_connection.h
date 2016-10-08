#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace GameEngine
{
namespace Networking
{
    // Keep the connection object alive as long as an object is referencing it
    class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
    {

    public:
        typedef boost::shared_ptr<TCPConnection> pointer;

		static pointer create(boost::asio::io_service& io_service) {
            return pointer(new TCPConnection(io_service));
        }

        boost::asio::ip::tcp::socket& socket() {
            return socket_;
        }

        void start() {
            message_ = "Message from TCPConnection!";

            boost::asio::async_write(socket_, boost::asio::buffer(message_),
                    boost::bind(&TCPConnection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }

        friend std::ostream& operator<<(std::ostream& os, const TCPConnection& connection) {
            return os << "[TCPConnection] ";
        }

    private:
        TCPConnection(boost::asio::io_service& io_service): socket_(io_service) {}

		void handle_write(const boost::system::error_code& /*error*/,
						  size_t /*bytes_transferred*/) { }

        boost::asio::ip::tcp::socket socket_;
        std::string message_;
    };
}
}


