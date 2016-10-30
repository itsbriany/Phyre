#pragma once
#include "tcp_client.h"

namespace GameEngine {
namespace Networking {

    class XMPPClient : public TCPClient {
        public:
            XMPPClient(boost::asio::io_service& io_service);

            void OnConnect() final;
            // void OnRead(const std::string& buffer) final;

            std::string log() override {
                return "[XMPPClient]";
            }

        private:
            enum TransactionState {
                kSelectAuthenticationMechanism
            };

            std::string xml_version();

            // The first stream used to authenticate
            std::string initiation_stream();

            // Authentication mechanism stream
            std::string authentication_mechanism();



            // All data read from endpoint ends up here
            std::string buffer_;

            // The current transactional state
            TransactionState state_;
    };

}
}
