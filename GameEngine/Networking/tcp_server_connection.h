#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <queue>
#include "logging.h"

namespace GameEngine
{
namespace Networking
{
    // Keep the connection object alive as long as an object is referencing it
    class TCPServerConnection : public std::enable_shared_from_this<TCPServerConnection>, public Logging::Loggable
    {

    public:
        typedef std::shared_ptr<TCPServerConnection> pointer;

        static pointer Create(boost::asio::io_service& io_service,
                              const std::queue<std::string>& message_queue = std::queue<std::string>());

        void Write();

        boost::asio::ip::tcp::socket& socket() { return socket_; }
        std::queue<std::string>& message_queue() { return message_queue_; }

        // Loggable overrides
        std::string log() override { return "[TCPServerConnection]"; }

    private:
        TCPServerConnection(boost::asio::io_service& io_service, const std::queue<std::string>& message_queue);
        void HandleWrite(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);
        void HandleRead(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);
        void HandleError(const boost::system::error_code&);


        // A buffer to store bytes read from client
        std::array<char, 4096> buffer_;

        // The socket used to write to the client
        boost::asio::ip::tcp::socket socket_;

        // This message is sent when all queue messages have been exhausted
        std::string default_message_;

        // A queue of messages which will be sent each time some bytes are
        // read from the client
        std::queue<std::string> message_queue_;
    };
}
}


