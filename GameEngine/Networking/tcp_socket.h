#pragma once
#include "loggable.h"
#include <boost/asio.hpp>

namespace GameEngine
{
namespace Networking
{
    class TCPSocket : public Logging::Loggable
    {
    public:

        typedef std::function<void(const boost::system::error_code&)> OnConnectCallback;
        typedef std::function<void(const boost::system::error_code&, size_t)> OnReadCallback;

        TCPSocket(boost::asio::io_service& io_service,
                  OnConnectCallback on_connect_callback,
                  OnReadCallback on_read_callback);
        ~TCPSocket();

        void Connect(boost::asio::ip::tcp::resolver::iterator it);
        void Close();

        /**
        * Each time this is called, a read operation will immediately be called afterwards.
        * It is therefore strongly advised to write all bytes to the data stream and read all necessary bytes
        * before calling Write again.
        */
        void Write(const std::string& data);
        void Read();


        /**
         * This TCP buffer has a window frame of 4kb
         */
        std::array<char, 4096>& buffer()  { return m_buffer; }
        bool is_connected() const { return m_is_connected; }

        std::string log() override {
            return "[TCPSocket]";
        }

    private:
        void OnConnect(const boost::system::error_code& ec);
        void OnRead(const boost::system::error_code& ec, size_t bytes_transferred);

        boost::asio::ip::tcp::socket m_socket;
        std::array<char, 4096> m_buffer;
        OnConnectCallback m_on_connect_callback;
        OnReadCallback m_on_read_callback;
        bool m_is_connected;
    };
}
}


