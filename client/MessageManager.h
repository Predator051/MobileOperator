#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "RRManager.h"
#include <utility>

class MessageManager
{
public:
    MessageManager(const std::string& address, const std::string& port);

    void start();    
    void setOnErrorCB(const std::function<void (ClientError error)> &onError);
    void setOnReadCB(const std::function<void (const network::ResponseContext&)> &onRead);
    void setOnConnected(const std::function<void ()> &onConnected);
    void execute(ByteBufferPtr buff);
    void execute(const std::string& buff);
    void execute(network::RequestContext &context);
    bool userAuth(const std::string& login, const std::string& password);
    bool createUser(const std::string& login, const std::string& password);
    bool logout();
    bool userStatus();
    bool userPackage(uint64_t user_id);
    bool userStory(uint64_t user_id);
    bool allRates();
    bool allService();
    bool allUsers();
    bool ratePayment(uint64_t rate_id);
    bool servicePayment(uint64_t rate_id);
    network::SessionInfo &sessionInfo();
    void setSessionInfo(const network::SessionInfo &sessionInfo);
    bool adminUpdateUserPackage(const network::AdminUpdateUserPackage& auup);
    bool adminRateChange(uint64_t rate_id, uint64_t user_id);
    bool adminServiceChange(uint64_t service_id, uint64_t user_id);
    bool rateStatistics();
    bool serviceStatistics();

private:

    std::shared_ptr<RRMananger> clientChatPtr_;
    asio::ssl::context context_;
    network::SessionInfo sessionInfo_;
};

#endif // MESSAGEMANAGER_H
