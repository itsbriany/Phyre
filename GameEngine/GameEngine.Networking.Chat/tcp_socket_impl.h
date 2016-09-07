#pragma once
#include "tcp_socket.h"

namespace engine
{
namespace networking
{
	class TCPSocketImpl : GameEngine::Networking::TCPSocket
	{
	public:
		TCPSocketImpl(boost::asio::io_service& io_service, OnReadCallback on_read_callback);
		~TCPSocketImpl();

		void Connect(boost::asio::ip::tcp::resolver::iterator it, OnConnectCallback& callback) override;
		void Close() override;
		void OnRead(const boost::system::error_code& ec, size_t bytes_transferred) override;
		void Write(std::string data) override;


		void set_on_read_callback(const OnReadCallback& on_read_callback) { on_read_callback_ = on_read_callback; }

	private:
		void AsyncRead();

		boost::asio::ip::tcp::socket socket_;
		std::vector<char> buffer_;
		OnReadCallback on_read_callback_;
	};
}
}


