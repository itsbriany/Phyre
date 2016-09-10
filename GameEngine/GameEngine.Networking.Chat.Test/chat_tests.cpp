#include <gmock/gmock.h>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "chat_client.h"
#include "mock_host_resolver.h"
#include "mock_tcp_socket.h"
#include "logging.h"

namespace GameEngine
{
namespace Networking
{

    using testing::AtLeast;
    using testing::_;
    using testing::Invoke;
    using testing::NiceMock;

    class ChatClientTest : public ::testing::Test {
    public:
        void OnConnectCallback() const
        {
            tcp_socket_->Read();
        }

        void OnHostResolvedCallback(const std::string& host, const std::string& service) const
        {
            boost::system::error_code ec;
            boost::asio::ip::tcp::resolver::iterator it;
            chat_client_->OnHostResolved(ec, it);
        }

        void OnHostNotFoundCallback(const std::string& host, const std::string& service) const
        {
            boost::asio::ip::tcp::resolver::iterator it;
            chat_client_->OnHostResolved(boost::asio::error::host_not_found, it);
        }

        void TCPSocketWriteStub(const std::string data_to_send) const
        {
            boost::system::error_code ec;
            tcp_socket_->buffer_.insert(tcp_socket_->buffer_.end(), data_to_send.begin(), data_to_send.end());
            tcp_socket_->OnRead(ec, data_to_send.size());
        }

    protected:
        void SetUp() override
        {
            boost::asio::io_service io_service;
            host_resolver_.reset(new MockHostResolver);
            tcp_socket_.reset(new NiceMock<MockTCPSocket>);
            chat_client_.reset(new ChatClient(io_service, *host_resolver_, *tcp_socket_));
        }

        std::unique_ptr<ChatClient> chat_client_;
        std::unique_ptr<MockHostResolver> host_resolver_;
        std::unique_ptr<NiceMock<MockTCPSocket>> tcp_socket_;
    };

    

    TEST_F(ChatClientTest, ConnectsToAServiceOnAHost)
    {
        std::string host = "localhost";
        std::string service = "http";

        EXPECT_CALL(*host_resolver_, ResolveHost(host, service, _));
        ON_CALL(*host_resolver_, ResolveHost(host, service, _)).WillByDefault(Invoke(boost::bind(&ChatClientTest::OnHostResolvedCallback, this, host, service)));
        EXPECT_CALL(*tcp_socket_, Connect(_, _));

        chat_client_->Connect(host, service);
    }

    TEST_F(ChatClientTest, ReadsAResponseAfterConnectingToAHost)
    {
        std::string host = "localhost";
        std::string service = "http";

        ON_CALL(*host_resolver_, ResolveHost(host, service, _)).WillByDefault(Invoke(boost::bind(&ChatClientTest::OnHostResolvedCallback, this, host, service)));
        EXPECT_CALL(*host_resolver_, ResolveHost(host, service, _));

        ON_CALL(*tcp_socket_, Connect(_, _)).WillByDefault(Invoke(boost::bind(&ChatClientTest::OnConnectCallback, this)));
        EXPECT_CALL(*tcp_socket_, Connect(_, _));
        EXPECT_CALL(*tcp_socket_, Read());

        chat_client_->Connect(host, service);
    }

    TEST_F(ChatClientTest, CallsAnErrorHandlerWhenItCannotConnectToAHost)
    {
        std::string host = "localhost";
        std::string service = "http";
        
        EXPECT_CALL(*host_resolver_, ResolveHost(host, service, _));
        ON_CALL(*host_resolver_, ResolveHost(host, service, _))
            .WillByDefault(Invoke(boost::bind(&ChatClientTest::OnHostNotFoundCallback, this, host, service)));
        EXPECT_CALL(*tcp_socket_, Connect(_, _)).Times(0);
        
        chat_client_->Connect(host, service);
    }

    TEST_F(ChatClientTest, ClosesConnectionOnError)
    {
        EXPECT_CALL(*tcp_socket_, Close());
        chat_client_->OnError(boost::asio::error::broken_pipe);
    }

    TEST_F(ChatClientTest, CanWriteWhenConnected)
    {
        boost::system::error_code ec;
        std::string data_to_send = "A string\n";
        
        EXPECT_CALL(*tcp_socket_, Write(data_to_send, _))
            .WillOnce(Invoke(boost::bind(&ChatClientTest::TCPSocketWriteStub,
                                         this,
                                         data_to_send)));
        
        chat_client_->OnConnect(ec);
        chat_client_->Write(data_to_send);
    }

    TEST_F(ChatClientTest, CannotWriteWhenDisconnected)
    {
        std::string data_to_send = "A string\n";

        EXPECT_CALL(*tcp_socket_, Write(data_to_send, _)).Times(0);

        chat_client_->Write(data_to_send);
    }
    
    TEST_F(ChatClientTest, HandlesReadAfterWrite)
    {
        std::string data_to_send = "some sting\n";
        boost::system::error_code ec;

        TCPSocket::OnReadCallback callback = boost::bind(&ChatClient::OnRead,
                                                         chat_client_.get(),
                                                         boost::asio::placeholders::error,
                                                         boost::asio::placeholders::bytes_transferred);
        EXPECT_CALL(*tcp_socket_, Write(data_to_send, _)).WillOnce(Invoke(boost::bind(&ChatClientTest::TCPSocketWriteStub, this, data_to_send)));
        EXPECT_CALL(*tcp_socket_, OnRead(ec, data_to_send.size()));

        chat_client_->OnConnect(ec);
        chat_client_->Write(data_to_send);
        EXPECT_FALSE(tcp_socket_->buffer().empty());
    }
}
}

int main(int ac, char* av[])
{
    GameEngine::Logging::disable_all();
    testing::InitGoogleMock(&ac, av);
    return RUN_ALL_TESTS();
}