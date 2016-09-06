#include <gmock/gmock.h>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "chat_client.h"
#include "mock_host_resolver.h"
#include "mock_tcp_socket.h"

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
}
}

int main(int ac, char* av[])
{
	testing::InitGoogleMock(&ac, av);
	return RUN_ALL_TESTS();
}