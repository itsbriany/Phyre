#include <boost/asio/io_service.hpp>
#include "chat_client.h"
#include "tcp_socket_impl.h"
#include "host_resolver_impl.h"
#include "logging.h"


namespace GameEngine
{
namespace Networking
{
    void ChatClientDemo()
    {
        Logging::set_log_level();
        
        boost::asio::io_service io_service;

        boost::asio::ip::tcp::resolver resolver(io_service);
        HostResolverImpl host_resolver(io_service, resolver);
        TCPSocketImpl socket(io_service);
        ChatClient cc(io_service, host_resolver, socket);
        cc.Connect("theboostcpplibraries.com", "80");
        io_service.run();

        // TODO: this will block the thread. Need to think of a better interface to handle asynchronous data
        while (!cc.is_connected()) { }
        cc.Write("GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n");
        
    }
}
}

int main(int argc, char* argv[])
{
    GameEngine::Networking::ChatClientDemo();
    return 0;
}
