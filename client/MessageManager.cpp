#include "MessageManager.h"
#include "Worker.h"
#include <string>
#include "Protobuf/Message.pb.h"
#include "Crypto/CryptoHelper.h"
#include "Config/GlobalParams.h"
#include "Cache/CacheManager.h"

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

void MessageManager::setOnConnected(const std::function<void ()> &onConnected)
{
    if(clientChatPtr_)
    {
        clientChatPtr_->setOnConnected(onConnected);
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

void MessageManager::execute(network::RequestContext &context)
{
    context.set_login(GlobalsParams::getSessionInfo().login());
    context.set_allocated_session_info(new network::SessionInfo(sessionInfo_));

    LOG_INFO("Send request " << context.message_type_());

    execute(context.SerializeAsString());
}

bool MessageManager::userAuth(const std::string &login, const std::string &password)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_AUTH);

    network::AuthMessage* authMess = new network::AuthMessage();
    authMess->set_login(login);
    GlobalsParams::getSessionInfo().set_login(login);

    std::string hashPass;
    CryptoHelper::md5_hash(password, hashPass);
    authMess->set_pass(hashPass);

    context.set_allocated_auth_message_(authMess);

    execute(context);
}

bool MessageManager::createUser(const std::string &login, const std::string &password)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_REGISTER);

    network::RegisterMessage* authMess = new network::RegisterMessage();
    authMess->set_login(login);
    GlobalsParams::getSessionInfo().set_login(login);

    std::string hashPass;
    CryptoHelper::md5_hash(password, hashPass);
    authMess->set_pass(hashPass);

    context.set_allocated_register_message_(authMess);

    execute(context);
}

bool MessageManager::logout()
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_LOGOUT);

    network::LogOutMessage* authMess = new network::LogOutMessage();
    authMess->set_login(GlobalsParams::getSessionInfo().login());

    context.set_allocated_logout_message(authMess);
    execute(context);
}

bool MessageManager::userStatus()
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_USER_STATUS);

    std::string lastSession = CacheManager::instance("").getLastSession();
    LOG_INFO("Last session: " << lastSession);
    if(lastSession.empty())
        return false;

    KeyValueMap cache = CacheManager::instance(lastSession).getCache();
    network::SessionInfo ses;
    ses.set_session_id(lastSession);
    ses.set_login(cache["login"]);
    ses.set_userid(atoi(cache["user_id"].c_str()));

    GlobalsParams::getSessionInfo() = sessionInfo_ = ses;
    execute(context);
}

bool MessageManager::userPackage(uint64_t user_id)
{
    network::RequestContext context;

    network::UserPackageInfoMessage* pack = new network::UserPackageInfoMessage();
    pack->set_user_id(user_id);
    context.set_allocated_package_info(pack);
    LOG_INFO("User id for package info " << user_id);
    context.set_message_type_(network::message_type::MO_USER_PACKAGE_INFO);
    execute(context);
}

bool MessageManager::userStory(uint64_t user_id)
{
    network::RequestContext context;
    network::UserStoryMessage* pack = new network::UserStoryMessage();
    pack->set_user_id(user_id);
    context.set_allocated_user_story(pack);
    LOG_INFO("User id for user story" << user_id);
    context.set_message_type_(network::message_type::MO_USER_STORY);
    execute(context);
}

bool MessageManager::allRates()
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_ALL_RATES);
    execute(context);
}

bool MessageManager::allService()
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_ALL_SERVICES);
    execute(context);
}

bool MessageManager::allUsers()
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_ALL_USERS);
    execute(context);
}

bool MessageManager::ratePayment(uint64_t rate_id)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_RATE_PAYMENT);

    network::RatePaymentMessage* authMess = new network::RatePaymentMessage();
    authMess->set_rate_id(rate_id);

    context.set_allocated_rate_payment(authMess);
    execute(context);
}

bool MessageManager::servicePayment(uint64_t rate_id)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_SERVICE_PAYMENT);

    network::ServicePaymentMessage* authMess = new network::ServicePaymentMessage();
    authMess->set_rate_id(rate_id);

    context.set_allocated_service_payment(authMess);
    execute(context);
}

network::SessionInfo& MessageManager::sessionInfo()
{
    return sessionInfo_;
}

void MessageManager::setSessionInfo(const network::SessionInfo &sessionInfo)
{
    sessionInfo_ = sessionInfo;
}

bool MessageManager::adminUpdateUserPackage(const network::AdminUpdateUserPackage &auup)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_ADMIN_UPDATE_USER_PACKAGE);

    network::AdminUpdateUserPackage* authMess = new network::AdminUpdateUserPackage(auup);

    context.set_allocated_admin_user_package(authMess);
    execute(context);
}

bool MessageManager::adminRateChange(uint64_t rate_id, uint64_t user_id)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_ADMIN_USER_RATE_CHANGE);

    network::AdminUserRateChange* authMess = new network::AdminUserRateChange();
    authMess->set_rate_id(rate_id);
     authMess->set_user_id(user_id);

    context.set_allocated_admin_user_rate_change(authMess);
    execute(context);
}

bool MessageManager::adminServiceChange(uint64_t service_id, uint64_t user_id)
{
    network::RequestContext context;
    context.set_message_type_(network::message_type::MO_ADMIN_USER_SERVICE_CHANGE);

    network::AdminUserServiceChange* authMess = new network::AdminUserServiceChange();
    authMess->set_service_id(service_id);
    authMess->set_user_id(user_id);

    context.set_allocated_admin_user_service_change(authMess);
    execute(context);
}
