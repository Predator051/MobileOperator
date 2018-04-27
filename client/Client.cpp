#include "Client.h"
#include "Worker.h"
#include "Helper.h"


Client::Client(std::string address, std::string port, asio::ssl::context& context)
    : io_service_(Worker::instance()->io_service())
    , address_(address)
    , port_(port)
    , resolver_(io_service_)
    , nextMsgSize_(0)
    , socket_(io_service_, context)
    , isVerified(false)
{
    buffer_.resize(2);
    socket_.set_verify_mode(asio::ssl::verify_peer);
    socket_.set_verify_callback(
                std::bind(&Client::verify_certificate
                          , this
                          , std::placeholders::_1
                          , std::placeholders::_2));

}

void Client::start()
{
    asio::ip::tcp::resolver::query query(address_, port_);
    asio::ip::tcp::resolver::iterator endpoint_iterator = resolver_.resolve(query);
    resolver_.async_resolve(query
                            , std::bind(&Client::handleResolveEndPoint
                                        , shared_from_this()
                                        , std::placeholders::_1
                                        , endpoint_iterator));

}

void Client::write(ByteBufferPtr buffPtr)
{
    ByteBufferPtr buffer = Helper::makeBuffer(buffPtr);
    asio::async_write(socket_
                      , asio::buffer(*buffer)
                      , std::bind(&Client::handleWrite
                                  , shared_from_this()
                                  , buffer
                                  , std::placeholders::_1
                                  , std::placeholders::_2));
}

void Client::handleResolveEndPoint(asio::error_code error, asio::ip::tcp::resolver::iterator iterator)
{

    if(!error)
    {
//        asio::ip::tcp::endpoint endPoint = *iterator;
//        socket_.async_connect(endPoint
//                              , std::bind(&Client::handleConnect
//                                          , shared_from_this()
//                                          , std::placeholders::_1
//                                          , ++iterator));
        asio::async_connect(socket_.lowest_layer(), iterator, std::bind(&Client::handleConnect
                                                                        , shared_from_this()
                                                                        , std::placeholders::_1
                                                                        , iterator));
    }
    else
    {
        LOG_ERR("Failure to resolve host adress: " << address_ << ":" << port_);
    }
}

void Client::handleConnect(asio::error_code error, asio::ip::tcp::resolver::iterator iterator)
{
    if(!error)
    {
        //read();
        socket_.async_handshake(asio::ssl::stream_base::client,
                                std::bind(&Client::handle_handshake, shared_from_this(),
                                            std::placeholders::_1));
    }
    else if(iterator != asio::ip::tcp::resolver::iterator())
    {
        asio::async_connect(socket_.lowest_layer(), iterator, std::bind(&Client::handleConnect
                                                                        , shared_from_this()
                                                                        , std::placeholders::_1
                                                                        , ++iterator));
    }
    else
    {
        LOG_ERR("Faiure: connect to address: "
                << address_ << ":" << port_ << " error: " <<error.message());
    }
}

bool Client::verify_certificate(bool preverified, asio::ssl::verify_context &ctx)
{
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";
    isVerified = preverified;
    return preverified;
}

void Client::read()
{
    if(0 == nextMsgSize_)
    {
        asio::async_read(socket_
                         , asio::buffer(buffer_, 2)
                         , asio::transfer_exactly(2)
                         , std::bind(&Client::handleRead
                                     , shared_from_this()
                                     , std::placeholders::_1
                                     , std::placeholders::_2));
    }
    else
    {
        asio::async_read(socket_
                         , asio::buffer(buffer_, nextMsgSize_)
                         , asio::transfer_at_least(nextMsgSize_)
                         , std::bind(&Client::handleRead
                                     , shared_from_this()
                                     , std::placeholders::_1
                                     , std::placeholders::_2));
    }

}

void Client::handle_handshake(const asio::error_code &error)
{
    if (!error)
    {
        read();
    }
    else
    {
        std::cout << "Handshake failed: " << error.message() << "\n";
    }
}

void Client::handleRead(std::error_code error, size_t bufferSize)
{
    if(!error)
    {
        if(0 == nextMsgSize_)
        {
            nextMsgSize_ = Helper::mergeTwoByte(buffer_[0], buffer_[1]);
            buffer_.resize(nextMsgSize_);
        }
        else
        {

            ByteBufferPtr buff = std::make_shared<ByteBuffer>(std::move(buffer_));

            onRead(buff);

            nextMsgSize_ = 0;

            buffer_.resize(2);
        }

        read();
    }
    else
    {
        LOG_ERR("Failure: read error code " << error.value()
                << " description: " << error.message());
    }
}

void Client::handleWrite(ByteBufferPtr data
                         , asio::error_code error
                         , size_t writeBytes)
{
    if(!error)
    {
    }
    else
    {
        LOG_ERR("Message write data"
                << Helper::bufferToString(data, 0, 0)
                << " description: "
                << error.message());
    }
}
