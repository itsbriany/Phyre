#pragma once
#include "tcp_client.h"

namespace GameEngine {
namespace Networking {

    class SASLHandshake {
        public:
            SASLHandshake(TCPClient& tcp_client);

        private:
            TCPClient& tcp_client_;
    };

}
}
