#ifndef CLIENT_H
#define CLIENT_H

#include "define.h"
#include "asio.hpp"
#include <asio/ssl.hpp>

class Client
        : public std::enable_shared_from_this<Client>
{
public:
    Client(std::string address, std::string port, asio::ssl::context &context);

    void start();

    void write(ByteBufferPtr buffPtr);
    virtual void onRead(ByteBufferPtr bufferPtr) = 0;
    virtual void onError(ClientError error) = 0;
    virtual void onConnected() = 0;

private:
    void handleResolveEndPoint(asio::error_code error, asio::ip::tcp::resolver::iterator iterator);
    void handleConnect(asio::error_code error, asio::ip::tcp::resolver::iterator iterator);
    bool verify_certificate(bool preverified, asio::ssl::verify_context& ctx);
    void read();
    void handle_handshake(const asio::error_code& error);
    void handleRead(std::error_code error, size_t bufferSize);

    void handleWrite(ByteBufferPtr data, asio::error_code error, size_t writeBytes);

    asio::io_service& io_service_;
    asio::ssl::stream<asio::ip::tcp::socket> socket_;

    std::string address_;
    std::string port_;

    asio::ip::tcp::resolver resolver_;

    ByteBuffer buffer_;

    uint16_t nextMsgSize_;
    std::atomic_bool isVerified;
};

#endif // CLIENT_H
