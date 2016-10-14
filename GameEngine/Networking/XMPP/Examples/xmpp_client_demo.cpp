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
        boost::asio::io_service io_service;
        std::string host = "0.0.0.0";
        std::string service = "5222";

        XMPPClient xmpp_client(io_service);
        xmpp_client.Connect(host, service);

        io_service.run();
    }
}
}

int main(int argc, char* argv[])
{
    GameEngine::Logging::set_log_level();
    GameEngine::Networking::XMPPClientDemo();
    return 0;
}
