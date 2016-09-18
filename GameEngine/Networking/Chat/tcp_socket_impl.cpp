#include <boost/bind/bind.hpp>
#include "tcp_socket_impl.h"
#include "chat_client.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{

    using boost::asio::ip::tcp;

    TCPSocketImpl::TCPSocketImpl(boost::asio::io_service& io_service) :
        socket_(tcp::socket(io_service)), buffer_(std::array<char, 4096>()), is_open_(false)
    {
    }

    TCPSocketImpl::~TCPSocketImpl()
    {
    }

    void TCPSocketImpl::Connect(tcp::resolver::iterator it, OnConnectCallback callback)
    {
        Logging::trace("Opening connection", *this);
        on_connect_callback_ = callback;
        socket_.async_connect(*it, boost::bind(&TCPSocketImpl::OnConnect, this, boost::asio::placeholders::error));
    }

    void TCPSocketImpl::OnConnect(const boost::system::error_code& ec)
    {
        is_open_ = true;
        on_connect_callback_(ec);
    }

    void TCPSocketImpl::Close()
    {
        Logging::debug("Closing TCP connection", *this);
        socket_.close();
        is_open_ = false;
    }

    void TCPSocketImpl::OnRead(const boost::system::error_code& ec, size_t bytes_transferred)
    {
        if (is_open_)
        {
            on_read_callback_(ec, bytes_transferred);
            AsyncRead();
        }
    }

    void TCPSocketImpl::AsyncRead()
    {
        if (is_open_)
        {
            socket_.async_read_some(boost::asio::buffer(buffer_, 4096),
                boost::bind(&TCPSocketImpl::OnRead,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }

    void TCPSocketImpl::Write(const std::string data, OnReadCallback on_read_callback)
    {
        if (!is_open_)
        {
            Logging::warning("Cannot write to closed socket", *this);
            return;
        }

        on_read_callback_ = on_read_callback;
        write(socket_, boost::asio::buffer(data));
        socket_.async_read_some(boost::asio::buffer(buffer_),
                                boost::bind(&TCPSocketImpl::OnRead,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
        
        std::ostringstream oss;
        oss << "Wrote " << data.size() << " bytes to socket:\n" << data;
        Logging::debug(oss.str(), *this);
    }

    void TCPSocketImpl::Read()
    {
        if (!is_open_)
        {
            Logging::warning("Cannot read from closed socket", *this);
            return;
        }
        AsyncRead();
    }
}
}
