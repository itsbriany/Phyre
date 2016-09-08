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

        virtual ~HostResolver() { }
        
        // Resolves a domains for a host on a certian port or service
        // The callback will be fired when the host and service have been resolved
        virtual void ResolveHost(const std::string& host,
                                 const std::string& service,
                                 OnHostResolvedCallback on_host_resolved_callback) = 0;
    };
}
}