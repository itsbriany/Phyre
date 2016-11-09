#pragma once
#include <memory>
#include "tcp_client.h"
#include "xmpp_state.h"

namespace GameEngine {
namespace Networking {

class XMPPState;
class XMPPClient : public TCPClient {
    public:
        XMPPClient(boost::asio::io_service& io_service, const std::string& host, const std::string& username, const std::string& password = "");

        // Called each time a connection is established
        void OnConnect() final;

        // Called every time some bytes are read from the endpoint
        void OnRead(const std::string& bytes_read) final;

        // Called each time a boost::asio error occurs
        void OnError(const boost::system::error_code& ec) final;

        void set_state(std::unique_ptr<XMPPState> p_state) { p_state_ = std::move(p_state); }

        std::string& buffer() { return buffer_; }

        std::string host() const { return host_; }

        std::string username() const { return username_; }

        std::string password() const { return password_; }

        std::string log() override {
            return "[XMPPClient]";
        }

    private:
        // All data read from endpoint ends up here
        std::string buffer_;

        // The current state in the lifetime of the XMPP protocol
        std::unique_ptr<XMPPState> p_state_;

        // The XMPP host
        std::string host_;

        // XMPP Username
        std::string username_;

        // XMPP Password
        std::string password_;
};

}
}
