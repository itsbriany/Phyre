#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <array>
#include "tcp_client.h"
#include "tcp_socket.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{

    using boost::asio::ip::tcp;

    TCPClient::TCPClient(boost::asio::io_service& io_service) :
        io_service_(io_service),
        host_resolver_(HostResolver(std::unique_ptr<tcp::resolver>(new tcp::resolver(io_service_)))),
        tcp_socket_(std::make_unique<TCPSocket>(io_service_,
                                                boost::bind(&TCPClient::ConnectHandler, this, boost::asio::placeholders::error),
                                                boost::bind(&TCPClient::ReadHandler,
                                                            this,
                                                            boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred),
                                                boost::bind(&TCPClient::WriteHandler,
                                                            this,
                                                            boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred))) { }

    TCPClient::~TCPClient() { }

    void TCPClient::Connect(const std::string& host, const std::string& service)
    {
        Logging::info("Connecting to " + host + ':' + service, *this);
        HostResolver::OnHostResolvedCallback callback = boost::bind(&TCPClient::ResolveHostHandler,
                                                                    this,
                                                                    boost::asio::placeholders::error,
                                                                    boost::asio::placeholders::iterator);
        host_resolver_.ResolveHost(host, service, callback);
    }

    void TCPClient::OnConnect() {
        Logging::info("Connected", *this);
    }

    void TCPClient::OnDisconnect() {
        Logging::info("Disconnected from endpoint", *this);
    }

    void TCPClient::Disconnect()
    {
        tcp_socket_->Close();
		TransactionComplete();
        OnDisconnect();
    }

    void TCPClient::ConnectHandler(const boost::system::error_code& ec) {
        if (ec) {
            ErrorHandler(ec);
            return;
        }

        Logging::info("Connection established", *this);
        OnConnect();
    }

    void TCPClient::OnRead(const std::string& data) {
        Logging::info(data, *this);
    }

    void TCPClient::ReadHandler(const boost::system::error_code& ec, size_t bytes_transferred) {
        if (ec)
        {
            ErrorHandler(ec);
            return;
        }

        std::ostringstream oss;
        oss << "Read " << bytes_transferred << " bytes";
        Logging::info(oss.str(), *this);

        OnRead(tcp_socket_->buffer().data());
    }

    void TCPClient::OnError(const boost::system::error_code& ec) {
        Logging::error(ec.message(), *this);
    }

    void TCPClient::ErrorHandler(const boost::system::error_code& ec) {
        OnError(ec);
        Disconnect();
    }

    void TCPClient::OnWrite(size_t bytes_transferred) {
        Logging::info("Transferred all bytes", *this);
    }

    void TCPClient::WriteHandler(const boost::system::error_code& error_code, size_t bytes_transferred) {
    	if (error_code) {
           ErrorHandler(error_code);
           return;
        }

        std::ostringstream log_output;
        log_output << "Sent " << bytes_transferred << " bytes to endpoint";
        Logging::info(log_output.str(), *this);

        OnWrite(bytes_transferred);
        tcp_socket_->Read();
    }

    void TCPClient::Write(const std::string& data) const {
        if (is_connected()) {
            tcp_socket_->Write(data);
            return;
        }

        Logging::warning("Attempting to write when no connection has been established", *this);
    }

	void TCPClient::TransactionComplete() const
	{
		tcp_socket_->ResetMessageQueue();
	}

	void TCPClient::OnHostResolved() {
        Logging::info("Host resolved", *this);
    }

    void TCPClient::ResolveHostHandler(const boost::system::error_code& ec, tcp::resolver::iterator it) {
        if (ec) {
            ErrorHandler(ec);
            return;
        }

        OnHostResolved();

        TCPSocket::OnConnectCallback callback = boost::bind(&TCPClient::ConnectHandler, this, boost::asio::placeholders::error);
        tcp_socket_->Connect(it);
    }
}
}


