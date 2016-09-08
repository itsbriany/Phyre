#include "chat_client.h"
#include <boost/asio/io_service.hpp>
#include "tcp_socket_impl.h"
#include <boost/bind/bind.hpp>
#include <iostream>

namespace GameEngine
{
namespace Networking
{

    void OnReadCallback(const boost::system::error_code& ec, const std::string data)
    {
        if (ec)
        {
            std::cout << "Error!\n";
            return;
        }

        std::cout << data << '\n';
    }

   
}
}

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    GameEngine::Networking::TCPSocketImpl socket(io_service);
    return 0;
}
