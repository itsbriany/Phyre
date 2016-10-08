#pragma once
#include "host_resolver.h"

namespace GameEngine
{
namespace Networking
{
    class HostResolverImpl : public HostResolver
    {
    public:
        HostResolverImpl(std::unique_ptr<boost::asio::ip::tcp::resolver> resolver);
        void ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback) override;

		friend std::ostream& operator<<(std::ostream& os, const HostResolverImpl& hri) {
			return os << "[HostResolverImpl] ";
		}

    private:
        std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_;
    };
}
}

