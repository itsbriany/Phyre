#pragma once
#include <boost/asio.hpp>
#include "host_resolver.h"
#include "tcp_socket.h"

namespace GameEngine
{
namespace Networking
{
    class TCPClient
    {

    public:
        TCPClient(boost::asio::io_service& io_service);
        virtual ~TCPClient();

        void Connect(const std::string& host, const std::string& service);
        void Disconnect();
        void Write(const std::string& data) const;
		
		// Invoke this to reset the message buffer once any network transaction with the server has been completed
		// This will free up all of the written messages bound to the lifetime of the TCP Socket
		void TransactionComplete() const;
        bool is_connected() const { return tcp_socket_->is_connected(); }

        friend std::ostream& operator<<(std::ostream& os, const TCPClient& cc) {
                return os << "[TCPClient] ";
        }

    protected:
        virtual void OnConnect();
        virtual void OnHostResolved();
        virtual void OnWrite(size_t bytes_transferred);
        virtual void OnRead(const std::string& data);
        virtual void OnDisconnect();
        virtual void OnError(const boost::system::error_code& ec);

        boost::asio::io_service& io_service_;

    private:
        void ConnectHandler(const boost::system::error_code& ec);
        void ResolveHostHandler(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it);
        void WriteHandler(const boost::system::error_code& ec, size_t bytes_transferred);
        void ReadHandler(const boost::system::error_code& ec, size_t bytes_transferred);
        void ErrorHandler(const boost::system::error_code& ec);

        HostResolver host_resolver_;
        std::unique_ptr<TCPSocket> tcp_socket_;
    };
}
}


