#pragma once
#include "host_resolver.h"

namespace GameEngine
{
namespace Networking
{
    class HostResolverImpl : public HostResolver
    {
    public:
        HostResolverImpl(boost::asio::io_service& io_service, boost::asio::ip::tcp::resolver& resolver);
        void ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback& callback) override;

    private:
        boost::asio::io_service& io_service_;
        boost::asio::ip::tcp::resolver& resolver_;
    };
}
}

