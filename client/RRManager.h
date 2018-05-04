#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include "Client.h"
#include <functional>

class RRMananger : public Client
{
public:
    RRMananger(std::string address, std::string port, asio::ssl::context &context);

    virtual void onRead(ByteBufferPtr bufferPtr) override;
    virtual void onError(ClientError error) override;
    void execute(ByteBufferPtr bufferPtr);

    void setOnErrorCB(const std::function<void (ClientError error)> &onError);

private:
    std::function<void (ClientError error)> onError_;
};

#endif // CLIENTCHAT_H
