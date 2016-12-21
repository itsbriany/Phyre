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
                         OnReadCallback on_read_callback) :
        m_socket(io_service),
        m_buffer(std::array<char, 4096>()),
        m_on_connect_callback(on_connect_callback),
        m_on_read_callback(on_read_callback),
        m_is_connected(false)
    {
    }

    TCPSocket::~TCPSocket()
    {
    }

    void TCPSocket::Connect(tcp::resolver::iterator it)
    {
        Logging::trace("Opening connection", *this);
        m_socket.async_connect(*it, boost::bind(&TCPSocket::OnConnect, this, boost::asio::placeholders::error));
    }

    void TCPSocket::OnConnect(const boost::system::error_code& ec)
    {
        m_is_connected = true;
        m_on_connect_callback(ec);
    }

    void TCPSocket::Close()
    {
        Logging::debug("Closing TCP connection", *this);
        m_socket.close();
        m_is_connected = false;
    }

    void TCPSocket::OnRead(const boost::system::error_code& ec, size_t bytes_transferred)
    {
        if (m_is_connected)
        {
            m_on_read_callback(ec, bytes_transferred);
            m_socket.async_read_some(boost::asio::buffer(m_buffer),
                                    boost::bind(&TCPSocket::OnRead,
                                                this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
    }

    void TCPSocket::Write(const std::string& data)
    {
        if (!m_is_connected)
        {
            Logging::warning("Cannot write to closed socket", *this);
            return;
        }
        write(m_socket, boost::asio::buffer(data));
        Read();
    }

    void TCPSocket::Read()
    {
        if (!m_is_connected)
        {
            Logging::warning("Cannot read from closed socket", *this);
            return;
        }
        m_socket.async_read_some(boost::asio::buffer(m_buffer),
                                boost::bind(&TCPSocket::OnRead,
                                    this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
    }
}
}
