#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include "Client.h"

class RRMananger : public Client
{
public:
    RRMananger(std::string address, std::string port, asio::ssl::context &context);

    virtual void onRead(ByteBufferPtr bufferPtr) override;
    void execute(CommandCode cmd, ByteBufferPtr bufferPtr);

private:

};

#endif // CLIENTCHAT_H
