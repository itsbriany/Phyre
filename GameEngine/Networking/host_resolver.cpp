#include "host_resolver.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{
    using boost::asio::ip::tcp;

    HostResolver::HostResolver(std::unique_ptr<tcp::resolver> resolver): m_resolver(std::move(resolver))
    {
    }

    void HostResolver::ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback)
    {
        tcp::resolver::query host_query(host, service);
        Logging::debug("Resolving " + host + ':' + service + "...", *this);
        m_resolver->async_resolve(host_query, callback);
    }
}
}
