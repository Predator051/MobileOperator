#ifndef CHATSESSION_H
#define CHATSESSION_H

#include "Session.h"

class ClientChannel : public Session
{
public:
    ClientChannel(asio::ssl::context &context);

    static std::shared_ptr<ClientChannel> getNewSessions(asio::ssl::context& context);

    virtual void onRead(ByteBuffer buffer) override;
    virtual void onDisconected() override;

    void setReadHandle(const std::function<void (std::shared_ptr<ClientChannel>, ByteBufferPtr)> &value);
    void setDisconectedHandle(const std::function<void (std::shared_ptr<ClientChannel>)> &disconectedHandle);
    void execute(CommandCode code, ByteBufferPtr data);
private:

    std::shared_ptr<ClientChannel> getPtrFromThis();

    std::function<void (std::shared_ptr<ClientChannel>, ByteBufferPtr)> readHandle_;
    std::function<void (std::shared_ptr<ClientChannel>)> disconectedHandle_;

};

typedef std::shared_ptr<ClientChannel> ClientChannelPtr;

#endif // SESSIONCHAT_H
