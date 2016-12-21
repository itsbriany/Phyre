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

        void set_state(std::unique_ptr<XMPPState> ptr_state) { m_ptr_state = std::move(ptr_state); }

        std::string& buffer() { return m_buffer; }

        std::string host() const { return m_host; }

        std::string username() const { return m_username; }

        std::string password() const { return m_password; }

        std::string log() override {
            return "[XMPPClient]";
        }

    private:
        // All data read from endpoint ends up here
        std::string m_buffer;

        // The XMPP host
        std::string m_host;

        // XMPP Username
        std::string m_username;

        // XMPP Password
        std::string m_password;

        // The current state in the lifetime of the XMPP protocol
        std::unique_ptr<XMPPState> m_ptr_state;
};

}
}
