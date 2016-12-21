#include "tcp_server_connection.h"

namespace GameEngine {
namespace Networking {

    TCPServerConnection::TCPServerConnection(boost::asio::io_service& io_service, const std::queue<std::string>& message_queue):
        m_socket(io_service),
        m_default_message("Greetings From TCPServerConnection!\r\n"),
        m_message_queue(message_queue) { }


    TCPServerConnection::pointer
    TCPServerConnection::Create(boost::asio::io_service& io_service, const std::queue<std::string>& message_queue) {
        return pointer(new TCPServerConnection(io_service, message_queue));
    }

    void TCPServerConnection::HandleError(const boost::system::error_code& error) {
        Logging::error(error.message(), *this);
        m_socket.close();
    }

    void TCPServerConnection::HandleRead(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            HandleError(error);
            return;
        }
        std::ostringstream oss;
        oss.write(m_buffer.data(), bytes_transferred);
        Logging::info(oss.str(), *this);
        Write();
    }

    void TCPServerConnection::Write() {
        std::string message;
        if (!m_message_queue.empty()) {
            message = m_message_queue.front();
            m_message_queue.pop();
        } else {
            message = m_default_message;
        }

        write(m_socket, boost::asio::buffer(message));

        // This is an asynchronous read, therefore, data may come in bursts.
        // i.e. multiple messages in the message queue may be sent at the same
        // time. However, the order in which the messages were queued in the
        // buffer should remain the same.
        m_socket.async_read_some(boost::asio::buffer(m_buffer),
                boost::bind(&TCPServerConnection::HandleRead, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

}
};
