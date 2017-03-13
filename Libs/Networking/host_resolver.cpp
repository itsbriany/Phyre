#include <Logging/logging.h>
#include "host_resolver.h"

namespace Phyre
{
namespace Networking
{
    using boost::asio::ip::tcp;
    const std::string HostResolver::kWho = "[HostResolver]";
    HostResolver::HostResolver(std::unique_ptr<tcp::resolver> resolver): ptr_resolver_(std::move(resolver))
    {
    }

    void HostResolver::ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback) const {
        tcp::resolver::query host_query(host, service);
        PHYRE_LOG(debug, kWho) << "Resolving " << host << ':' << service << "...";
        ptr_resolver_->async_resolve(host_query, callback);
    }
}
}
