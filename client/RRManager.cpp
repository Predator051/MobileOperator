#include "RRManager.h"
#include <functional>
#include <Helper.h>
#include <algorithm>
#include <mutex>
#include <Protobuf/Message.pb.h>
#include "boost/signals2.hpp"

RRMananger::RRMananger(std::string address, std::string port, asio::ssl::context& context)
    : Client(address, port, context)
{

}

void RRMananger::onRead(ByteBufferPtr bufferPtr)
{
    std::string buff(bufferPtr->begin(), bufferPtr->end());
    network::ResponseContext resCntx;
    resCntx.ParseFromString(buff);

    LOG_INFO(resCntx.error_code() << " " << resCntx.message_type_())

    if(onRead_)
    {
        onRead_(resCntx);
    }
}

void RRMananger::onError(ClientError error)
{
    if(onError_)
    {
        onError_(error);
    }
}

void RRMananger::onConnected()
{
    if(onConnected_)
    {
        onConnected_();
    }
}

void RRMananger::execute(ByteBufferPtr bufferPtr)
{
    write(bufferPtr);
}

void RRMananger::setOnErrorCB(const std::function<void (ClientError error)> &onError)
{
    onError_ = onError;
}

void RRMananger::setOnRead(const std::function<void (const network::ResponseContext &)> &onRead)
{
    onRead_ = onRead;
}

void RRMananger::setOnConnected(const std::function<void ()> &onConnected)
{
    onConnected_ = onConnected;
}
