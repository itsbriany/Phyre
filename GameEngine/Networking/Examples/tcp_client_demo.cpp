#include <boost/asio/io_service.hpp>
#include "tcp_client.h"
#include "tcp_socket.h"
#include "host_resolver.h"
#include "logging.h"


namespace GameEngine
{
namespace Networking
{
    class TCPClientDemo : public TCPClient {
        public:
            TCPClientDemo(boost::asio::io_service& io_service) : TCPClient(io_service) { }

            void Start() {
                std::string host = "theboostcpplibraries.com";
                std::string port_or_service = "80";
                Connect(host, port_or_service);
                io_service_.run();
            }

            void OnConnect() override {
                std::string payload1 = "GET / HTTP/1.1\r\n";
                std::string payload2 = "Host: theboostcpplibraries.com\r\n\r\n";
                Write(payload1);
                Write(payload2);
            }
    };
}
}

int main(int argc, char* argv[])
{
    GameEngine::Logging::set_log_level();
    boost::asio::io_service io_service;
    GameEngine::Networking::TCPClientDemo client(io_service);
    client.Start();
    return 0;
}

/*
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <array>
#include <string>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::resolver resolv{ioservice};
tcp::socket tcp_socket{ioservice};
std::array<char, 4096> bytes;

void read_handler(const boost::system::error_code &ec,
  std::size_t bytes_transferred)
{
  if (!ec)
  {
    std::cout.write(bytes.data(), bytes_transferred);
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void write_handler(const boost::system::error_code& ec,
        std::size_t bytes_transferred) {
    tcp_socket.async_read_some(buffer(bytes), read_handler);
}

void connect_handler(const boost::system::error_code &ec)
{
  if (!ec)
  {
    std::string r1 = "GET / HTTP/1.1\r\n";
    std::string r2 = "Host: theboostcpplibraries.com\r\n\r\n";
    write(tcp_socket, buffer(r1));
    write(tcp_socket, buffer(r2));
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void resolve_handler(const boost::system::error_code &ec,
  tcp::resolver::iterator it)
{
  if (!ec)
    tcp_socket.async_connect(*it, connect_handler);
}

int main()
{
  tcp::resolver::query q{"theboostcpplibraries.com", "80"};
  resolv.async_resolve(q, resolve_handler);
  ioservice.run();
}
*/
