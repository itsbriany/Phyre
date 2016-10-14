#include <gmock/gmock.h>
#include <boost/bind/bind.hpp>
#include "tcp_client.h"
#include "fake_tcp_clients.h"
#include "tcp_server.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{

    class TCPClientTest : public ::testing::Test {
    protected:
        TCPClientTest():
            host_("0.0.0.0"),
            port_or_service_("1234"),
            tcp_server_(TCPServer(io_service_, std::stoi(port_or_service_))) { }

        virtual ~TCPClientTest() { }

        void SetUp() override { }

        boost::asio::io_service io_service_;
        std::string host_;
        std::string port_or_service_;
        TCPServer tcp_server_;
    };

    TEST_F(TCPClientTest, CanResolveHosts) {
        TCPClientHostResolved client(io_service_);
        tcp_server_.StartAccept();
        client.Connect(host_, port_or_service_);
        io_service_.run();
    }

    TEST_F(TCPClientTest, CanConnectToAService) {
        TCPClientConnect client(io_service_);
        tcp_server_.StartAccept();
        client.Connect(host_, port_or_service_);
        io_service_.run();
    }

    TEST_F(TCPClientTest, CanWriteToAService) {
        std::string payload = "Hello!\r\n";
        TCPClientWrite client(io_service_, payload);
        tcp_server_.StartAccept();
        client.Connect(host_, port_or_service_);
        io_service_.run();
    }

    TEST_F(TCPClientTest, CanReadFromAService) {
        std::string payload = "Hello!\r\n";
        TCPClientRead client(io_service_, payload);
        tcp_server_.StartAccept();
        client.Connect(host_, port_or_service_);
        io_service_.run();
    }

    TEST_F(TCPClientTest, CanDisconnectFromAService) {
        TCPClientDisconnect client(io_service_);
        tcp_server_.StartAccept();
        client.Connect(host_, port_or_service_);
        io_service_.run();
    }

    TEST_F(TCPClientTest, DisconnectsAfterError) {
        TCPClientError client(io_service_);

        // There is no server to connect to, so an error is handled
        client.Connect(host_, port_or_service_);
        io_service_.run();
    }
}
}

int main(int argc, char* argv[]) {
    // GameEngine::Logging::disable_all();
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
