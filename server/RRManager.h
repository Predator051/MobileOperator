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
    ResponseCode packageInfoRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode userStoryRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode allServicesRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode allRatesRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode ratePaymentRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode servicePaymentRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode allUsersRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode updateUserPackage(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode adminRateChangeRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode adminServiceChangeRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode adminRateStatisticsRR(const network::RequestContext& requests, network::ResponseContext &response);
    ResponseCode adminServiceStatisticsRR(const network::RequestContext& requests, network::ResponseContext &response);

    std::vector<ClientChannelPtr> sessions_;
};

#endif // CHATMANAGER_H
