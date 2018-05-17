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
    void execute(ByteBufferPtr buff);
    void execute(const std::string& buff);
    bool userAuth(const std::string& login, const std::string& password);
    bool createUser(const std::string& login, const std::string& password);
private:

    std::shared_ptr<RRMananger> clientChatPtr_;
    asio::ssl::context context_;
};

#endif // MESSAGEMANAGER_H
