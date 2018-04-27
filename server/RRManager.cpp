#include "RRManager.h"
#include "Helper.h"
#include "Protobuf/Message.pb.h"

RRManager::RRManager(Server& server)
{
    server.subscribe(std::bind(
                         &RRManager::onConnected
                         , this
                         , std::placeholders::_1));
}

void RRManager::onConnected(ClientChannelPtr session)
{
    LOG_INFO("Connected session");
    sessions_.emplace_back(session);

    session->setReadHandle(std::bind(&RRManager::readSessionBuffer
                                     , this
                                     , std::placeholders::_1
                                     , std::placeholders::_2));

    session->setDisconectedHandle(std::bind(&RRManager::disconectedSession
                                            , this
                                            , std::placeholders::_1));
}

void RRManager::readSessionBuffer(std::shared_ptr<ClientChannel> session, ByteBufferPtr buffPtr)
{
    std::string str(buffPtr->begin(), buffPtr->end());
    sample::proto::Message mes;
    mes.ParseFromString(str);
    LOG_INFO("ID " << mes.id() << " DATA: " << mes.data());
    session->execute("I got it!");
}

void RRManager::disconectedSession(std::shared_ptr<ClientChannel> session)
{
    LOG_INFO("Disconected!");
}
