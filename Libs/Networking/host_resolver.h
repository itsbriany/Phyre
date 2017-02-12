#pragma once
#include "loggable_interface.h"
#include <boost/asio.hpp>

namespace Phyre
{
namespace Networking
{
    class HostResolver : public Logging::LoggableInterface
    {
    public:
        typedef std::function<void(const boost::system::error_code&, boost::asio::ip::tcp::resolver::iterator)> OnHostResolvedCallback;

        HostResolver(std::unique_ptr<boost::asio::ip::tcp::resolver> resolver);
        void ResolveHost(const std::string& host, const std::string& service, OnHostResolvedCallback callback);

        std::string log() override {
            return "[HostResolver]";
        }

    private:
        std::unique_ptr<boost::asio::ip::tcp::resolver> ptr_resolver_;
    };
}
}

