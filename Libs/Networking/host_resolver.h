#pragma once

#include <boost/asio.hpp>
#include <Logging/loggable_interface.h>

namespace Phyre {
namespace Networking {
    class HostResolver
    {
    public:
        typedef std::function<void(const boost::system::error_code&, boost::asio::ip::tcp::resolver::iterator)> OnHostResolvedCallback;

        HostResolver(std::unique_ptr<boost::asio::ip::tcp::resolver> resolver);
        void ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback) const;

    private:
        std::unique_ptr<boost::asio::ip::tcp::resolver> ptr_resolver_;
        static const std::string kWho;
    };
}
}

