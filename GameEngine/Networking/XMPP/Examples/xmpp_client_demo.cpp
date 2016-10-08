#include "xmpp_client.h"
#include "tcp_socket.h"
#include "host_resolver.h"
#include "logging.h"


namespace GameEngine
{
namespace Networking
{
    void XMPPClientDemo()
    {
        Logging::set_log_level();

        boost::asio::io_service io_service;
        std::string host = "0.0.0.0";
        std::string service = "5222";
        std::string data = "foo\r\n";

        std::unique_ptr<TCPClient> tcp_client = TCPClient::MakeTCPClient(io_service, host, service);
        tcp_client->Connect(host, service, data);

        /*
        XMPPClient xmpp_client(host, service, tcp_client);
        xmpp_client.Connect();
        */

        io_service.run();
    }
}
}

int main(int argc, char* argv[])
{
    GameEngine::Networking::XMPPClientDemo();
    return 0;
}
