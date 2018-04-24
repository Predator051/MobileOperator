#ifndef SERVER_H
#define SERVER_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include "ClientChannel.h"

class Server
{
public:
    Server(const std::string &ip, int port);

    void start();

    void subscribe(std::function<void(ClientChannelPtr)> cb);
private:
    void accept();
    void handleAccept(ClientChannelPtr session, asio::error_code error);
    std::string get_password() const
    {
      return "12345";
    }
    asio::io_service& io_service_;
    asio::ip::tcp::acceptor acceptor_;
    asio::ssl::context context_;

    std::vector<ClientChannelPtr> sessions_;

    std::vector<std::function<void(ClientChannelPtr)>> onConnectedCbs;
};

#endif // SERVER_H
