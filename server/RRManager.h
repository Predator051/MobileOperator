#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include "Server.h"
#include "ClientChannel.h"
#include "Protobuf/Message.pb.h"

class RRManager
{
public:
    RRManager(Server &server);

    void onConnected(ClientChannelPtr session);

private:
    void readSessionBuffer(std::shared_ptr<ClientChannel> session, ByteBufferPtr buffPtr);
    void disconectedSession(std::shared_ptr<ClientChannel> session);
    void saveSession(std::shared_ptr<ClientChannel> session
                     , const network::ResponseContext &response);

    ResponseCode checkSession(network::RequestContext& request, network::ResponseContext& response);

    ResponseCode authRR(const network::RequestContext& request, network::ResponseContext& response);
    ResponseCode registerRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode logOutRR(const network::RequestContext& requests);
    std::vector<ClientChannelPtr> sessions_;
};

#endif // CHATMANAGER_H
