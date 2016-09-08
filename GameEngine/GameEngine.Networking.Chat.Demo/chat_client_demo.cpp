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
        Logging::config();
        Logging::trace("Trace!");
        Logging::debug("Debug!");
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        HostResolverImpl host_resolver(io_service, resolver);
        TCPSocketImpl socket(io_service);
        ChatClient cc(io_service, host_resolver, socket);
        cc.Connect("www.boost.org/doc/libs/1_61_0/doc/html/boost_asio.html", "http");
        io_service.run();
    }
}
}

int main(int argc, char* argv[])
{
    GameEngine::Networking::ChatClientDemo();
    return 0;
}
