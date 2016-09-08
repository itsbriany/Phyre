#include "host_resolver_impl.h"

namespace GameEngine
{
namespace Networking
{
    using boost::asio::ip::tcp;

    HostResolverImpl::HostResolverImpl(boost::asio::io_service& io_service, tcp::resolver& resolver) :
        io_service_(io_service), resolver_(resolver)
    {
    }

    void HostResolverImpl::ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback)
    {
        tcp::resolver::query host_query(host, service);
        resolver_.async_resolve(host_query, callback);
    }
}
}
