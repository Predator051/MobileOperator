#include "MessageManager.h"
#include "Worker.h"
#include <string>

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

    std::string message;
    std::cin >> message;
    std::vector<uint8_t> vec(message.begin(), message.end());
    clientChatPtr_->execute(CommandCode::ANSWER_ON_REQUEST_TO_CONNECT
                            , std::make_shared<ByteBuffer>(vec));

    Worker::instance()->join();
}
