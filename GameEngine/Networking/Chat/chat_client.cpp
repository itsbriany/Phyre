#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <array>
#include "chat_client.h"
#include "tcp_socket.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{

    using boost::asio::ip::tcp;

    ChatClient::ChatClient(boost::asio::io_service& io_service, HostResolver& resolver, TCPSocket& tcp_socket) :
        io_service_(io_service), host_resolver_(resolver), tcp_socket_(tcp_socket), is_connected_(false)
    {
    }

    ChatClient::~ChatClient()
    {
    }

    void ChatClient::Connect(const std::string& host, const std::string& service, const std::string& data_to_send)
    {
        Logging::info("Connecting to " + host + ':' + service, *this);
        data_to_send_on_connect_ = data_to_send;
        HostResolver::OnHostResolvedCallback callback = boost::bind(&ChatClient::OnHostResolved,
                                                                    this,
                                                                    boost::asio::placeholders::error,
                                                                    boost::asio::placeholders::iterator);
        host_resolver_.ResolveHost(host, service, callback);
    }

    void ChatClient::Disconnect() const
    {
        tcp_socket_.Close();
        Logging::info("Disconnected from endpoint", *this);
    }

    // TODO: On connect should write some data to the server
    void ChatClient::OnConnect(const boost::system::error_code& ec)
    {
        if (ec)
        {
            OnError(ec);
            return;
        }

        is_connected_ = true;
        Logging::info("Connection established", *this);
        Write(data_to_send_on_connect_);
    }

    void ChatClient::OnRead(const boost::system::error_code& ec, size_t bytes_transferred)
    {
        if (ec)
        {
            OnError(ec);
            return;
        }

        // TODO: Probably want to start reading from the previous read + bytes_transferred
        std::ostringstream oss;
        oss.write(tcp_socket_.buffer().data(), bytes_transferred);
        Logging::info(oss.str(), *this);
    }

    void ChatClient::OnError(const boost::system::error_code& ec)
    {
        Logging::error(ec.message(), *this);

        // TODO: We may also not always want to close the connection if the error is not severe enough
        is_connected_ = false;

        Logging::info("Closing connection", *this);
        tcp_socket_.Close();
    }

    void ChatClient::Write(const std::string data)
    {
        std::ostringstream log_output;

        if (is_connected_)
        {
            log_output << "Sending " << data.size() << " bytes to endpoint";
            Logging::info(log_output.str(), *this);
            tcp_socket_.Write(data, boost::bind(&ChatClient::OnRead,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
            return;
        }

        Logging::warning("Attempting to write when no connection has been established", *this);
    }

    void ChatClient::OnHostResolved(const boost::system::error_code& ec, tcp::resolver::iterator it)
    {
        if (ec)
        {
            OnError(ec);
            return;
        }

        TCPSocket::OnConnectCallback callback = boost::bind(&ChatClient::OnConnect, this, boost::asio::placeholders::error);
        tcp_socket_.Connect(it, callback);
    }
}
}

