#pragma once
#include <boost/property_tree/ptree.hpp>
#include <b64/encode.h>
#include <b64/decode.h>
#include <unordered_set>
#include <unordered_map>
#include "xmpp_state.h"
#include "logging.h"

namespace GameEngine {
namespace Networking {

class SASL : public XMPPState, public Logging::Loggable {
    public:
        enum TransactionState {
            kInitializeStream,
            kSelectAuthenticationMechanism,
            kDecodeBase64Challenge
        };

        // Authentication mechanisms in SASL
        enum Mechanism {
            kSHA1,
            kMD5,
            kNone
        };

        SASL(XMPPClient& client);

        // XMPPState overrides
        void Update() override;

        // Loggable overrides
        std::string log() override { return "[SASL]"; }

        // Extract XML from buffer
        std::stringstream ExtractXML(const std::string& start_tag, const std::string& end_tag) const;

        // Extract the authentication mechanisms from buffer
        std::stringstream ExtractAuthenticationMechanismResponse() const;

        void HandleInitializeStream();
        void HandleSelectAuthenticationMechanism();
        void HandleDecodeBase64Challenge();
        std::unordered_set<std::string> ParseAuthenticationMechanisms(std::istream& xml_stream) const;
        std::string ParseBase64Challenge(std::istream& xml_stream) const;
        std::string EncodeBase64(const std::string& input);
        std::string DecodeBase64(const std::string& input);
        std::ostringstream InitiateAuthenticationStream(Mechanism mechanism);
        TransactionState transaction_state() const { return m_transaction_state; }
        void set_transaction_state(TransactionState transaction_state) { m_transaction_state = transaction_state; }

        static std::string GenerateNonce();

    private:
        static Mechanism SetAuthenticationMechanism(const std::unordered_set<std::string>& authentication_mechanism_set);
        static void ParseAuthenticationMechanismsRecursive(const boost::property_tree::ptree& pt,
                                                           std::unordered_set<std::string>& authentication_mechanism_set);

        static bool IsMD5AuthenticationMechanismAvailable(const std::unordered_set<std::string>& authentication_mechanism_set);
        static bool IsSHA1AuthenticationMechanismAvailable(const std::unordered_set<std::string>& authentication_mechanism_set);
        std::string SHA1AuthenticationPayload();

        // XML version used in SASL
        static std::ostringstream xml_version();

        // The first stream used to authenticate
        std::ostringstream initiation_stream() const;

        // Map a digest to a string
        static std::unordered_map<Mechanism, std::string> s_digest_map;

        // A base64 encoder
        base64::encoder m_base64_encoder;

        // A base64 decoder
        base64::decoder m_base64_decoder;

        // Internal transactional state
        TransactionState m_transaction_state;
};

}
}
