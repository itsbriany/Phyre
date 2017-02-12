#include <boost/asio/io_service.hpp>
#include "tcp_server.h"
#include "logging.h"


namespace Phyre
{
namespace Networking
{
    void TCPServerDemo()
    {
        Logging::set_log_level();
        try
        {
            boost::asio::io_service io_service;
            uint16_t port = 1234;
            TCPServer server(io_service, port);
            server.StartAccept();
            io_service.run();
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}
}

int main(int argc, char* argv[])
{
    Phyre::Networking::TCPServerDemo();
    return 0;
}
