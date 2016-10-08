#include "host_resolver_impl.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{
    using boost::asio::ip::tcp;

    HostResolverImpl::HostResolverImpl(std::unique_ptr<tcp::resolver> resolver): resolver_(std::move(resolver))
    {
    }

    void HostResolverImpl::ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback)
    {
        tcp::resolver::query host_query(host, service);
        Logging::debug("Resolving " + host + ':' + service + "...", *this);
        resolver_->async_resolve(host_query, callback);
    }
}
}
