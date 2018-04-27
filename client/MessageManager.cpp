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

    std::string messageID;
    std::cin >> messageID;
    sample::proto::Message mes;
    mes.set_id(messageID);

    std::string messageData;
    std::cin >> messageData;
    mes.set_data(messageData);
    std::string outMessage = mes.SerializeAsString();
    std::vector<uint8_t> vec(outMessage.begin(), outMessage.end());
    clientChatPtr_->execute(std::make_shared<ByteBuffer>(vec));

    Worker::instance()->join();
}
