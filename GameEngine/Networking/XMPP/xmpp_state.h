#pragma once
#include "xmpp_client.h"

namespace GameEngine {
namespace Networking {

class XMPPClient;
class XMPPState {
    public:
        XMPPState(XMPPClient& client): m_client(client) {}
        virtual ~XMPPState() { }

        // Updates the state of the XMPP client
        virtual void Update() = 0;

    protected:
        XMPPClient& m_client;
};

}
}
