#include <boost/assign.hpp>
#include <gtest/gtest.h>
#include "fake_tcp_clients.h"
#include "logging.h"
#include "tcp_client.h"
#include "tcp_server.h"

namespace GameEngine
{
namespace Networking
{
    class TCPClientTest : public ::testing::Test {
    protected:
        TCPClientTest():
            host_("127.0.0.1"),
            port_or_service_("1234"),
            tcp_server_(TCPServer(io_service_, std::stoi(port_or_service_))) { }

        virtual ~TCPClientTest() { }

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

    TEST_F(TCPClientTest, CanReadAndWriteFromAService) {
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

    TEST_F(TCPClientTest, ServerSendsQueuedMessages) {
        std::queue<std::string> message_queue;
        boost::assign::push(message_queue)("first")("second")("third")("fourth")("fith");
        std::string port_or_service = "1235";

        tcp_server_ = TCPServer(io_service_, std::stoi(port_or_service), message_queue);
        TCPClientReadQueuedMessages client(io_service_, message_queue);

        tcp_server_.StartAccept();
        client.Connect(host_, port_or_service);
        io_service_.run();
    }

}
}