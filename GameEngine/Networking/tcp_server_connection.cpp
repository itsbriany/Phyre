#include "tcp_server_connection.h"

namespace GameEngine {
namespace Networking {

    TCPServerConnection::TCPServerConnection(boost::asio::io_service& io_service, const std::queue<std::string>& message_queue):
        socket_(io_service),
        default_message_("Greetings From TCPServerConnection!\r\n"),
        message_queue_(message_queue) { }


    TCPServerConnection::pointer
    TCPServerConnection::Create(boost::asio::io_service& io_service, const std::queue<std::string>& message_queue) {
        return pointer(new TCPServerConnection(io_service, message_queue));
    }

    void TCPServerConnection::HandleError(const boost::system::error_code& error) {
        Logging::error(error.message(), *this);
        socket_.close();
    }

    // Be careful when writing data here; large chunks of transmitted data
    // will invoke multiple HandleRead calls
    void TCPServerConnection::HandleRead(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            HandleError(error);
            return;
        }
        std::ostringstream oss;
        oss.write(buffer_.data(), bytes_transferred);
        Logging::info(oss.str(), *this);
        Write();
    }

    void TCPServerConnection::Write() {
        std::string message;
        if (!message_queue_.empty()) {
            message = message_queue_.front();
            message_queue_.pop();
        } else {
            message = default_message_;
        }

        write(socket_, boost::asio::buffer(message));

        socket_.async_read_some(boost::asio::buffer(buffer_),
                boost::bind(&TCPServerConnection::HandleRead, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

}
};
