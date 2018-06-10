#ifndef CHATSESSION_H
#define CHATSESSION_H

#include "Session.h"
#include "Types/UserInfo.h"
#include <mutex>

class ClientChannel : public Session
{
public:
    ClientChannel(asio::ssl::context &context);

    static std::shared_ptr<ClientChannel> getNewSessions(asio::ssl::context& context);

    virtual void onRead(ByteBuffer buffer) override;
    virtual void onDisconected() override;

    void setReadHandle(const std::function<void (std::shared_ptr<ClientChannel>, ByteBufferPtr)> &value);
    void setDisconectedHandle(const std::function<void (std::shared_ptr<ClientChannel>)> &disconectedHandle);
    void execute(ByteBufferPtr data);
    void execute(const std::string& data);
    UserInfo getUserInfo() const;
    void setUserInfo(const UserInfo &value);

    std::string getSessionId() const;
    void setSessionId(const std::string &value);

private:

    std::shared_ptr<ClientChannel> getPtrFromThis();

    std::function<void (std::shared_ptr<ClientChannel>, ByteBufferPtr)> readHandle_;
    std::function<void (std::shared_ptr<ClientChannel>)> disconectedHandle_;

    UserInfo userInfo;
    std::string sessionId;
    std::mutex mutex_;
};

typedef std::shared_ptr<ClientChannel> ClientChannelPtr;

#endif // SESSIONCHAT_H
