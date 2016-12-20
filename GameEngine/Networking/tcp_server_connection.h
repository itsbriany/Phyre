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

        boost::asio::ip::tcp::socket& socket() { return m_socket; }
        std::queue<std::string>& message_queue() { return m_message_queue; }

        // Loggable overrides
        std::string log() override { return "[TCPServerConnection]"; }

    private:
        TCPServerConnection(boost::asio::io_service& io_service, const std::queue<std::string>& message_queue);
        void HandleWrite(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);
        void HandleRead(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);
        void HandleError(const boost::system::error_code&);


        // A buffer to store bytes read from client
        std::array<char, 4096> m_buffer;

        // The socket used to write to the client
        boost::asio::ip::tcp::socket m_socket;

        // This message is sent when all queue messages have been exhausted
        std::string m_default_message;

        // A queue of messages which will be sent each time some bytes are
        // read from the client
        std::queue<std::string> m_message_queue;
    };
}
}


