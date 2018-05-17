#include "MessageManager.h"
#include "Worker.h"
#include <string>
#include "Protobuf/Message.pb.h"

MessageManager::MessageManager(const std::string &address, const std::string &port)
    : context_(asio::ssl::context::sslv23)
{
    context_.load_verify_file("server.crt");
    clientChatPtr_ = std::make_shared<RRMananger>(address, port, context_);
}

void MessageManager::start()
{
    clientChatPtr_->start();
    Worker::instance()->start();
    Worker::instance()->join();
}

void MessageManager::setOnErrorCB(const std::function<void (ClientError)> &onError)
{
    if(clientChatPtr_)
    {
        clientChatPtr_->setOnErrorCB(onError);
    }
}

void MessageManager::setOnReadCB(const std::function<void (const network::ResponseContext &)> &onRead)
{
    if(clientChatPtr_)
    {
        clientChatPtr_->setOnRead(onRead);
    }
}

void MessageManager::execute(ByteBufferPtr buff)
{
    if(clientChatPtr_ && buff)
    {
        clientChatPtr_->execute(buff);
    }
}

void MessageManager::execute(const std::string &buff)
{
    ByteBufferPtr buffPtr = std::make_shared<ByteBuffer>(buff.begin(), buff.end());

    execute(buffPtr);
}

bool MessageManager::userAuth(const std::string &login, const std::string &password)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_AUTH);

    network::AuthMessage* authMess = new network::AuthMessage();
    authMess->set_login(login);
    authMess->set_pass(password);

    context.set_allocated_auth_message_(authMess);

    execute(context.SerializeAsString());
}

bool MessageManager::createUser(const std::string &login, const std::string &password)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_REGISTER);

    network::RegisterMessage* authMess = new network::RegisterMessage();
    authMess->set_login(login);
    authMess->set_pass(password);

    context.set_allocated_register_message_(authMess);

    execute(context.SerializeAsString());
}
