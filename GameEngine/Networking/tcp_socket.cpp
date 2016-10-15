#include <boost/bind/bind.hpp>
#include "tcp_socket.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{

    using boost::asio::ip::tcp;

    TCPSocket::TCPSocket(boost::asio::io_service& io_service, 
    					 OnConnectCallback on_connect_callback,
						 OnReadCallback on_read_callback,
						 OnWriteCallback on_write_callback) :
        socket_(io_service),
        buffer_(std::array<char, 4096>()),
		on_connect_callback_(on_connect_callback),
		on_read_callback_(on_read_callback),
		on_write_callback_(on_write_callback),
        is_connected_(false)
    {
    }

    TCPSocket::~TCPSocket()
    {
    }

    void TCPSocket::Connect(tcp::resolver::iterator it)
    {
        Logging::trace("Opening connection", *this);
        socket_.async_connect(*it, boost::bind(&TCPSocket::OnConnect, this, boost::asio::placeholders::error));
    }

    void TCPSocket::OnConnect(const boost::system::error_code& ec)
    {
        is_connected_ = true;
        on_connect_callback_(ec);
    }

    void TCPSocket::Close()
    {
        Logging::debug("Closing TCP connection", *this);
        socket_.close();
        is_connected_ = false;
    }

    void TCPSocket::OnRead(const boost::system::error_code& ec, size_t bytes_transferred)
    {
        if (is_connected_)
        {
            on_read_callback_(ec, bytes_transferred);
            socket_.async_read_some(boost::asio::buffer(buffer_),
                                    boost::bind(&TCPSocket::OnRead,
                                                this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
    }

	void TCPSocket::Write(const std::string& data)
    {
        if (!is_connected_)
        {
            Logging::warning("Cannot write to closed socket", *this);
            return;
        }
		message_queue_.push(data);
        boost::asio::async_write(socket_, boost::asio::buffer(message_queue_.back()), on_write_callback_);

        std::ostringstream oss;
        oss << "Wrote " << data.size() << " bytes to socket:\n" << data;
        Logging::debug(oss.str(), *this);
    }

    void TCPSocket::Read()
    {
        if (!is_connected_)
        {
            Logging::warning("Cannot read from closed socket", *this);
            return;
        }
        socket_.async_read_some(boost::asio::buffer(buffer_),
                                boost::bind(&TCPSocket::OnRead,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
    }

	void TCPSocket::ResetMessageQueue(std::queue<std::string> new_message_queue)
	{
		message_queue_ = new_message_queue;
	}
}
}
