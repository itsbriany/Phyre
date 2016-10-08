#pragma once
#include "tcp_client.h"
#include "sasl_handshake.h"

namespace GameEngine {
namespace Networking {

    class XMPPClient {
        public:
            XMPPClient(const std::string& xmpp_host, const std::string& service, TCPClient& tcp_client);
            void Connect();

        private:
            std::string xmpp_host_;
            std::string xmpp_service_;
            TCPClient& tcp_client_;
            SASLHandshake* sasl_handshake_;
    };

}
}
