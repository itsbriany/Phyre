#pragma once
#include "tcp_socket.h"

namespace engine
{
namespace networking
{
	class TCPSocketImpl : GameEngine::Networking::TCPSocket
	{
	public:
		TCPSocketImpl(boost::asio::io_service& io_service);
		~TCPSocketImpl();

		void Connect(boost::asio::ip::tcp::resolver::iterator it, OnConnectCallback& callback) override;
		void Close() override;
		void OnRead(const boost::system::error_code& ec, size_t bytes_transferred) override;
		void Write(const std::string& data, OnWriteCallback callback) override;
		

	private:
		void AsyncRead();

		boost::asio::ip::tcp::socket socket_;
		std::vector<char> buffer_;
	};
}
}


