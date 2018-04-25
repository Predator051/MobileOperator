#include "ClientChat.h"
#include <functional>
#include <Helper.h>
#include <algorithm>
#include <mutex>

ClientChat::ClientChat(std::string address, std::string port, asio::ssl::context& context)
    : Client(address, port, context)
{

}

void ClientChat::onRead(ByteBufferPtr bufferPtr)
{
}

void ClientChat::execute(CommandCode cmd, ByteBufferPtr bufferPtr)
{
    write(bufferPtr);
}
