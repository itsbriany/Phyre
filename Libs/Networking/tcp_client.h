#pragma once
#include <boost/asio.hpp>
#include "host_resolver.h"
#include "tcp_socket.h"

namespace Phyre
{
namespace Networking
{
    class TCPClient : public Logging::LoggableInterface
    {

    public:
        TCPClient(boost::asio::io_service& io_service);
        virtual ~TCPClient();

        void Connect(const std::string& host, const std::string& service);
        void Disconnect();
        void Write(const std::ostringstream& data_stream);

        bool is_connected() const { return ptr_tcp_socket_->is_connected(); }

        std::string log() override {
            return "[TCPClient]";
        }

    protected:
        virtual void OnConnect();
        virtual void OnHostResolved();
        virtual void OnRead(const std::string& buffer);
        virtual void OnDisconnect();
        virtual void OnError(const boost::system::error_code& ec);

        boost::asio::io_service& io_service_;

    private:
        void ConnectHandler(const boost::system::error_code& ec);
        void ResolveHostHandler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it);
        void ReadHandler(const boost::system::error_code& ec, size_t bytes_transferred);
        void ErrorHandler(const boost::system::error_code& ec);

        HostResolver host_resolver_;
        std::unique_ptr<TCPSocket> ptr_tcp_socket_;
    };
}
}


