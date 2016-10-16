#pragma once
#include <boost/asio.hpp>

namespace GameEngine
{
namespace Networking
{
    class HostResolver
    {
    public:
        typedef std::function<void(const boost::system::error_code&, boost::asio::ip::tcp::resolver::iterator)> OnHostResolvedCallback;

        HostResolver(std::unique_ptr<boost::asio::ip::tcp::resolver> resolver);
        void ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback);

        friend std::ostream& operator<<(std::ostream& os, const HostResolver& host_resolver) {
            return os << "[HostResolver] ";
        }

    private:
        std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_;
    };
}
}

