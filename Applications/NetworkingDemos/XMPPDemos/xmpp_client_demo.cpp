#include "xmpp_client.h"
#include "tcp_socket.h"
#include "host_resolver.h"
#include "logging.h"


namespace Phyre
{
namespace Networking
{
    void XMPPClientDemo()
    {
        boost::asio::io_service io_service;
        std::string host = "dev-server";
        std::string service = "5222";
        std::string username = "admin";
        std::string password = "password";

        XMPPClient xmpp_client(io_service, host, username, password);
        xmpp_client.Connect(host, service);

        io_service.run();
    }
}
}

int main(int argc, char* argv[])
{
    Phyre::Logging::set_log_level();
    Phyre::Networking::XMPPClientDemo();
    return 0;
}
