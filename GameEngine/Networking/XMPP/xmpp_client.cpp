#include "logging.h"
#include "sasl.h"
#include "xmpp_client.h"

namespace GameEngine {
namespace Networking {

XMPPClient::XMPPClient(boost::asio::io_service& io_service,
                       const std::string& host,
                       const std::string& username,
                       const std::string& password):
        TCPClient(io_service),
        m_host(host),
        m_username(username),
        m_password(password),
        m_ptr_state(std::make_unique<SASL>(*this)) { }

void XMPPClient::OnConnect() {
    m_ptr_state->Update();
}

void XMPPClient::OnRead(const std::string& bytes_read) {
    std::string message = "From server: \n" + std::string(bytes_read);
    Logging::debug(message, *this);
    m_buffer += bytes_read;
    m_ptr_state->Update();
}

void XMPPClient::OnError(const boost::system::error_code& ec) {
    Logging::error(ec.message(), *this);
}

}
}
