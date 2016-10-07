#include <boost/asio/io_service.hpp>
#include "tcp_client.h"
#include "tcp_socket_impl.h"
#include "host_resolver_impl.h"
#include "logging.h"


namespace GameEngine
{
namespace Networking
{
    void TCPClientDemoHTTP()
    {
        Logging::set_log_level();

        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        HostResolverImpl host_resolver(io_service, resolver);
        TCPSocketImpl socket(io_service);
        TCPClient cc(io_service, host_resolver, socket);

        std::string host = "theboostcpplibraries.com";
        std::string port_or_service = "80";
        std::string data_to_send_on_connect = "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";

        cc.Connect(host, port_or_service, data_to_send_on_connect);

        io_service.run();
    }
}
}

int main(int argc, char* argv[])
{
    GameEngine::Networking::TCPClientDemoHTTP();
    return 0;
}
