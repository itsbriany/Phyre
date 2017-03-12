#include <boost/asio/io_service.hpp>
#include <Networking/tcp_client.h>
#include <Networking/host_resolver.h>
#include <Logging/logging.h>


namespace Phyre {
namespace Networking {

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
            std::ostringstream oss;
            oss << "GET / HTTP/1.1\r\n";
            oss << "Host: theboostcpplibraries.com\r\n\r\n";
            Write(oss);
        }
};
}
}

int main() {
    Phyre::Logging::set_log_level();
    boost::asio::io_service io_service;
    Phyre::Networking::TCPClientDemo client(io_service);
    client.Start();
    return 0;
}
