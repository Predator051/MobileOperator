#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include "Server.h"
#include "ClientChannel.h"

class RRManager
{
public:
    RRManager(Server &server);

    void onConnected(ClientChannelPtr session);

private:
    void readSessionBuffer(std::shared_ptr<ClientChannel> session, ByteBufferPtr buffPtr);
    void disconectedSession(std::shared_ptr<ClientChannel> session);

    void onLoginError(std::shared_ptr<ClientChannel> session, const std::string &text);

    std::vector<ClientChannelPtr> sessions_;
};

#endif // CHATMANAGER_H
