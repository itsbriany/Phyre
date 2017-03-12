#pragma once
#include "xmpp_client.h"

namespace Phyre {
namespace Networking {

class XMPPClient;
class XMPPState {
    public:
        XMPPState(XMPPClient& client): client_(client) {}
        virtual ~XMPPState() { }

        // Updates the state of the XMPP client
        virtual void Update() = 0;

    protected:
        XMPPClient& client_;
};

}
}
