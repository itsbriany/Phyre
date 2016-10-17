#include <gtest/gtest.h>
#include "logging.h"
#include "tcp_client.h"

namespace GameEngine {
namespace Networking {

class TCPClientConnect : public TCPClient {
    public:
        TCPClientConnect(boost::asio::io_service& io_service):
            TCPClient(io_service) { }

    protected:
        void OnConnect() override {
            io_service_.stop();
        }
};

class TCPClientHostResolved : public TCPClient {
    public:
        TCPClientHostResolved(boost::asio::io_service& io_service):
            TCPClient(io_service) { }

    protected:
        void OnHostResolved() override {
            io_service_.stop();
        }
};

class TCPClientRead : public TCPClient {
    public:
        TCPClientRead(boost::asio::io_service& io_service, const std::string& payload):
            TCPClient(io_service),
            payload_(payload) { }


        friend std::ostream& operator<<(std::ostream& os, const TCPClientRead& client) {
            return os << "[TCPClientRead] ";
        }


        std::string payload_;

    protected:
        void OnConnect() override {
            Write(payload_);
        }

        void OnRead(const std::string& data, size_t bytes_transferred) override {
            Logging::info(data, *this);
            io_service_.stop();
        }
};

class TCPClientDisconnect : public TCPClient {
    public:
        TCPClientDisconnect(boost::asio::io_service& io_service):
            TCPClient(io_service) { }


    protected:
        void OnConnect() override {
            Disconnect();
        }

        void OnDisconnect() override {
            EXPECT_FALSE(is_connected());
            io_service_.stop();
        }
};

class TCPClientError : public TCPClient {
    public:
        TCPClientError(boost::asio::io_service& io_service):
            TCPClient(io_service) { }

    protected:
        void OnError(const boost::system::error_code& ec) {
            EXPECT_TRUE(ec);
        }

        void OnDisconnect() override {
            io_service_.stop();
        }
};

}
}
