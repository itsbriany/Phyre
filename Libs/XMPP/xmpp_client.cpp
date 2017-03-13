#include <Logging/logging.h>
#include "sasl.h"
#include "xmpp_client.h"

namespace Phyre {
namespace Networking {

    const std::string XMPPClient::kWho = "[XMPPClient]";

XMPPClient::XMPPClient(boost::asio::io_service& io_service,
                       const std::string& host,
                       const std::string& username,
                       const std::string& password):
        TCPClient(io_service),
        host_(host),
        username_(username),
        password_(password),
        ptr_state_(std::make_unique<SASL>(*this)) { }

void XMPPClient::OnConnect() {
    ptr_state_->Update();
}

void XMPPClient::OnRead(const std::string& bytes_read) {
    std::string message = "From server: \n" + std::string(bytes_read);
    PHYRE_LOG(debug, kWho) << message;
    buffer_ += bytes_read;
    ptr_state_->Update();
}

void XMPPClient::OnError(const boost::system::error_code& ec) {
    PHYRE_LOG(error, kWho) << ec.message();
}

}
}
