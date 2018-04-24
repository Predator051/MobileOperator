#ifndef SESSION_H
#define SESSION_H

#include <memory>
#include <asio.hpp>
#include "define.h"
#include "asio/ssl.hpp"
#include <asio/system_error.hpp>

typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_socket;

class Session
        : public std::enable_shared_from_this<Session>
{

public:
    Session(asio::ssl::context& context);

    void start();

    ssl_socket::lowest_layer_type &socket();

    void write(ByteBufferPtr message);

    virtual void onRead(ByteBuffer buffer) = 0;
    virtual void onDisconected() = 0;
    void handle_handshake(const asio::error_code& error);
    virtual ~Session() {}

    void stop();

private:
    void read();

    void handleRead(asio::error_code error, size_t bufferSize);
    void handleWrite(ByteBufferPtr data, asio::error_code error, size_t writedBytesCount);

    ssl_socket socket_;
    ByteBuffer buffer_;
    uint16_t nextMsgSize_;
    bool isStop = false;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif // SESSION_H
