#include "Server.h"
#include "define.h"
#include "Worker.h"

Server::Server(const std::string& ip, int port)
    : io_service_(Worker::instance()->io_service())
    , acceptor_( io_service_
                 , asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    , context_(asio::ssl::context::sslv23)

{
    LOG_INFO("Created server for port: " << port);
    context_.set_options(
                asio::ssl::context::default_workarounds
                | asio::ssl::context::no_sslv2
                | asio::ssl::context::single_dh_use);
    context_.set_password_callback(std::bind(&Server::get_password, this));
    context_.use_certificate_chain_file("server.crt");
    context_.use_private_key_file("server.key", asio::ssl::context::pem);
    context_.use_tmp_dh_file("dh512.pem");
}

void Server::accept()
{

    ClientChannelPtr session = ClientChannel::getNewSessions(context_);

    acceptor_.async_accept(session->socket(), std::bind(
                               &Server::handleAccept
                               , this
                               , session
                               , std::placeholders::_1));
}

void Server::handleAccept(ClientChannelPtr session, asio::error_code error)
{

    if(!error)
    {
        asio::ip::tcp::endpoint  client_addr = session->socket().remote_endpoint();
        LOG_INFO("Client connected: "
                 << client_addr.address().to_string()
                 << ":"
                 << client_addr.port());
        sessions_.push_back(session);
        session->start();

        for (std::function<void(ClientChannelPtr)> cb : onConnectedCbs)
        {
            cb(session);
        }
    }

    accept();
}

void Server::start()
{
    accept();
}

void Server::subscribe(std::function<void (ClientChannelPtr)> cb)
{
    onConnectedCbs.emplace_back(cb);
}

