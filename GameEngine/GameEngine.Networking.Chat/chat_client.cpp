#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "chat_client.h"
#include "tcp_socket.h"
#include <iostream>

namespace GameEngine
{
namespace Networking
{
	
	using boost::asio::ip::tcp;

	ChatClient::ChatClient(boost::asio::io_service& io_service, HostResolver& resolver, TCPSocket& tcp_socket) :
        io_service_(io_service), host_resolver_(resolver), tcp_socket_(tcp_socket)
	{
	}

	ChatClient::~ChatClient()
	{
	}

	void ChatClient::Connect(const std::string& host, const std::string& service)
	{
        HostResolver::OnHostResolvedCallback callback = boost::bind(&ChatClient::OnHostResolved,
                                                                    this,
                                                                    boost::asio::placeholders::error,
                                                                    boost::asio::placeholders::iterator);
		host_resolver_.ResolveHost(host, service, callback);
	}

	void ChatClient::OnConnect(const boost::system::error_code& ec)
	{
		if (ec)
		{
			OnError(ec);
			return;
		}

		std::string message = "Hello from Boost ASIO\r\n";
		tcp_socket_.Write(message, boost::bind(&ChatClient::OnRead, this, boost::asio::placeholders::error, boost::placeholders::_2));
	}

	void ChatClient::OnRead(const boost::system::error_code& ec, const std::string& data)
	{
		
		if (ec)
		{
			OnError(ec);
			return;
		}

		// TODO: This should be logged instead
		// TODO: Read from the tcp socket buffer
		std::cout << data << "\n";
		/*if (!ec)
		{
			std::cout.write(bytes_.data(), bytes_transferred);
			socket_.async_read_some(boost::asio::buffer(bytes_),
				boost::bind(&ChatClient::OnRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}*/
	}

    void ChatClient::OnError(const boost::system::error_code& ec)
    {
		// TODO: Log the error code
		// TODO: We may also not always want to close the connection if the error is not severe enough

		tcp_socket_.Close();
    }

    void ChatClient::OnHostResolved(const boost::system::error_code& ec, tcp::resolver::iterator it)
	{
        if (ec)
        {
            OnError(ec);
            return;
        }
        
        TCPSocket::OnConnectCallback callback = boost::bind(&ChatClient::OnConnect, this, boost::asio::placeholders::error);
        tcp_socket_.Connect(it, callback);
	}
}
}


