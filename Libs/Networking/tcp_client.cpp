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
        host_resolver_(HostResolver(std::make_unique<tcp::resolver>(io_service_))),
        ptr_tcp_socket_(std::make_unique<TCPSocket>(io_service_,
                                                boost::bind(&TCPClient::ConnectHandler, this, boost::asio::placeholders::error),
                                                boost::bind(&TCPClient::ReadHandler,
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
        ptr_tcp_socket_->Close();
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
        std::cout << data;
    }

    void TCPClient::ReadHandler(const boost::system::error_code& ec, size_t bytes_transferred) {
        if (ec) {
            ErrorHandler(ec);
            return;
        }

        std::ostringstream read_buffer;
        read_buffer.write(ptr_tcp_socket_->buffer().data(), bytes_transferred);
        OnRead(read_buffer.str());
    }

    void TCPClient::OnError(const boost::system::error_code& ec) {
        Logging::error(ec.message(), *this);
    }

    void TCPClient::ErrorHandler(const boost::system::error_code& ec) {
        OnError(ec);
        Disconnect();
    }

    void TCPClient::Write(const std::ostringstream& data_stream) {
        if (is_connected()) {
            std::string data(data_stream.str());
            ptr_tcp_socket_->Write(data);
            std::ostringstream log_output;
            log_output << "Sent " << data.size() << " bytes to endpoint:\n";
            log_output << data;
            Logging::info(log_output.str(), *this);
            return;
        }

        Logging::warning("Attempting to write when no connection has been established", *this);
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
        ptr_tcp_socket_->Connect(it);
    }
}
}


