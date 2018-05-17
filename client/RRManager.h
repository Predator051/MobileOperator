#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include "Client.h"
#include <functional>
#include <Protobuf/Message.pb.h>

class RRMananger : public Client
{
public:
    RRMananger(std::string address, std::string port, asio::ssl::context &context);

    virtual void onRead(ByteBufferPtr bufferPtr) override;
    virtual void onError(ClientError error) override;
    void execute(ByteBufferPtr bufferPtr);

    void setOnErrorCB(const std::function<void (ClientError error)> &onError);

    void setOnRead(const std::function<void (const network::ResponseContext&)> &onRead);

private:
    std::function<void (ClientError error)> onError_;
    std::function<void (const network::ResponseContext&)> onRead_;
};

#endif // CLIENTCHAT_H
