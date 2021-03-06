#include <boost/bind/bind.hpp>
#include <Logging/logging.h>
#include "tcp_socket.h"

namespace Phyre {
namespace Networking {

    using boost::asio::ip::tcp;

    const std::string TCPSocket::kWho = "[TCPSocket]";

    TCPSocket::TCPSocket(boost::asio::io_service& io_service,
                         OnConnectCallback on_connect_callback,
                         OnReadCallback on_read_callback) :
        socket_(io_service),
        buffer_(std::array<char, 4096>()),
        on_connect_callback_(on_connect_callback),
        on_read_callback_(on_read_callback),
        is_connected_(false)
    {
    }

    TCPSocket::~TCPSocket()
    {
    }

    void TCPSocket::Connect(tcp::resolver::iterator it)
    {
        PHYRE_LOG(trace, kWho) << "Opening connection";
        socket_.async_connect(*it, boost::bind(&TCPSocket::OnConnect, this, boost::asio::placeholders::error));
    }

    void TCPSocket::OnConnect(const boost::system::error_code& ec)
    {
        is_connected_ = true;
        on_connect_callback_(ec);
    }

    void TCPSocket::Close()
    {
        PHYRE_LOG(debug, kWho) << "Closing TCP connection";
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
            PHYRE_LOG(warning, kWho) << "Cannot write to closed socket";
            return;
        }
        write(socket_, boost::asio::buffer(data));
        Read();
    }

    void TCPSocket::Read()
    {
        if (!is_connected_)
        {
            PHYRE_LOG(warning, kWho) << "Cannot read from closed socket";
            return;
        }
        socket_.async_read_some(boost::asio::buffer(buffer_),
                                boost::bind(&TCPSocket::OnRead,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
    }
}
}
