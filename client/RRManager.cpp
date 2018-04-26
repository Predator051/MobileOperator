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
}

void RRMananger::execute(CommandCode cmd, ByteBufferPtr bufferPtr)
{
    write(bufferPtr);
}
