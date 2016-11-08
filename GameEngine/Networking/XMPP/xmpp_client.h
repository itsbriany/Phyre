#pragma once
#include <memory>
#include "tcp_client.h"
#include "xmpp_state.h"

namespace GameEngine {
namespace Networking {

class XMPPState;
class XMPPClient : public TCPClient {
    public:
        XMPPClient(boost::asio::io_service& io_service);
        XMPPClient(XMPPClient&& other);

        // Called each time a connection is established
        void OnConnect() final;

        // Called every time some bytes are read from the endpoint
        void OnRead(const std::string& bytes_read) final;

        // Called each time a boost::asio error occurs
        void OnError(const boost::system::error_code& ec) final;

        void set_state(std::unique_ptr<XMPPState> p_state) { p_state_ = std::move(p_state); }

        std::string& buffer() { return buffer_; }

        std::string log() override {
            return "[XMPPClient]";
        }

    private:
        // All data read from endpoint ends up here
        std::string buffer_;

        // The current state in the lifetime of the XMPP protocol
        std::unique_ptr<XMPPState> p_state_;
};

}
}
