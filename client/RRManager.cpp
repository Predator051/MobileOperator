#include "RRManager.h"
#include <functional>
#include <Helper.h>
#include <algorithm>
#include <mutex>

RRMananger::RRMananger(std::string address, std::string port, asio::ssl::context& context)
    : Client(address, port, context)
{

}

void RRMananger::onRead(ByteBufferPtr bufferPtr)
{
    std::string buff(bufferPtr->begin(), bufferPtr->end());
    LOG_INFO(buff);
}

void RRMananger::execute(ByteBufferPtr bufferPtr)
{
    write(bufferPtr);
}
