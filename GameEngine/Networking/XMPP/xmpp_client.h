#pragma once
#include "tcp_client.h"
#include "sasl_handshake.h"

namespace GameEngine {
namespace Networking {

    class XMPPClient : public TCPClient {
        public:
            XMPPClient(boost::asio::io_service& io_service);
            void OnConnect() final;
    };

}
}
