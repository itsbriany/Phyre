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
        /*
        std::unique_ptr<boost::asio::ip::tcp::resolver> resolver(new boost::asio::ip::tcp::resolver(io_service));
        std::unique_ptr<HostResolverImpl> host_resolver(new HostResolverImpl(std::move(resolver)));
        std::unique_ptr<TCPSocketImpl> socket(new TCPSocketImpl(io_service));
        TCPClient cc(std::move(host_resolver), std::move(socket));
        */


        std::string host = "theboostcpplibraries.com";
        std::string port_or_service = "80";
        std::string data_to_send_on_connect = "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";

        std::unique_ptr<TCPClient> cc = TCPClient::MakeTCPClient(io_service, host, port_or_service);

        cc->Connect(host, port_or_service, data_to_send_on_connect);

        io_service.run();
    }
}
}

int main(int argc, char* argv[])
{
    GameEngine::Networking::TCPClientDemoHTTP();
    return 0;
}
