#include <Logging/logging.h>
#include "host_resolver.h"

namespace Phyre
{
namespace Networking
{
    using boost::asio::ip::tcp;

    HostResolver::HostResolver(std::unique_ptr<tcp::resolver> resolver): ptr_resolver_(std::move(resolver))
    {
    }

    void HostResolver::ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback)
    {
        tcp::resolver::query host_query(host, service);
        Logging::debug("Resolving " + host + ':' + service + "...", *this);
        ptr_resolver_->async_resolve(host_query, callback);
    }
}
}
