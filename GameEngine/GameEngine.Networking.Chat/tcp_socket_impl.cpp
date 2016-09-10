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
        socket_(tcp::socket(io_service)), buffer_(std::vector<char>())
    {
    }

    TCPSocketImpl::~TCPSocketImpl()
    {
    }

    void TCPSocketImpl::Connect(tcp::resolver::iterator it, OnConnectCallback callback)
    {
        Logging::trace("Opening connection", *this);
        socket_.async_connect(*it, callback);
    }

    void TCPSocketImpl::Close()
    {
        Logging::trace("Closing connection", *this);
        socket_.close();
    }

    void TCPSocketImpl::OnRead(const boost::system::error_code& ec, size_t bytes_transferred)
    {
        on_read_callback_(ec, bytes_transferred);
        AsyncRead();
    }

    void TCPSocketImpl::AsyncRead()
    {
        socket_.async_read_some(boost::asio::buffer(buffer_),
                                boost::bind(&TCPSocketImpl::OnRead,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void TCPSocketImpl::Write(const std::string data, OnReadCallback on_read_callback)
    {
        on_read_callback_ = on_read_callback;
        write(socket_, boost::asio::buffer(data));
        
        std::ostringstream oss;
        oss << "Wrote " << data.size() << " bytes to socket:\n" << data;
        Logging::debug(oss.str(), *this);

        AsyncRead();
    }

    void TCPSocketImpl::Read()
    {
        AsyncRead();
    }
}
}
