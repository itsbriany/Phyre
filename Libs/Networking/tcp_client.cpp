#include <array>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <Logging/logging.h>
#include "tcp_client.h"
#include "tcp_socket.h"

namespace Phyre {
namespace Networking {

    using boost::asio::ip::tcp;
    const std::string TCPClient::kWho = "[TCPClient]";

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
        PHYRE_LOG(info, kWho) << "Connecting to " << host << ':' << service;
        HostResolver::OnHostResolvedCallback callback = boost::bind(&TCPClient::ResolveHostHandler,
                                                                    this,
                                                                    boost::asio::placeholders::error,
                                                                    boost::asio::placeholders::iterator);
        host_resolver_.ResolveHost(host, service, callback);
    }

    void TCPClient::OnConnect() {
        PHYRE_LOG(info, kWho) << "Connected";
    }

    void TCPClient::OnDisconnect() {
        PHYRE_LOG(info, kWho) << ": Disconnected from endpoint";
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

        PHYRE_LOG(info, kWho) << ": Connection established";
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
        PHYRE_LOG(error, kWho) << ec.message();
    }

    void TCPClient::ErrorHandler(const boost::system::error_code& ec) {
        OnError(ec);
        Disconnect();
    }

    void TCPClient::Write(const std::ostringstream& data_stream) {
        if (is_connected()) {
            std::string data(data_stream.str());
            ptr_tcp_socket_->Write(data);
            PHYRE_LOG(info, kWho) << "Sent " << data.size() << " bytes to endpoint:\n";
            return;
        }

        PHYRE_LOG(warning, kWho) << "Attempting to write when no connection has been established";
    }

    void TCPClient::OnHostResolved() {
        PHYRE_LOG(info, kWho) << "Host resolved";
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


