#include "RRManager.h"
#include "Helper.h"
#include "Logic/AuthLogic.h"
#include "db/SessionPostgresInfo.h"
#include "db/AuthPostgresManager.h"

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
    network::RequestContext reqContext;
    network::ResponseContext resContext;
    reqContext.ParseFromString(Helper::bufferToString(buffPtr, 0, 0));
    LOG_INFO("Request [" << reqContext.message_type_()
             << "] from [" << reqContext.login());

    ResponseCode responseCode;
    do
    {
        if(!reqContext.has_session_info() && reqContext.message_type_() != network::MO_AUTH)
        {
            LOG_INFO("User trying to do with out login");
            responseCode = ResponseCode::status_bad_request;
            break;
        }

        responseCode = checkSession(reqContext, resContext);
        if(responseCode != ResponseCode::status_success)
        {
            LOG_ERR("Failure to check session!");
            break;
        }

        UserInfo  uInfo;
        if(AuthPostgresManager::getUserByLogin(reqContext.login(), uInfo) == ResponseCode::status_success)
        {
            LOG_INFO("User id [" << uInfo.user_id << "]");
            session->setUserInfo(uInfo);
        }

        switch (reqContext.message_type_()) {
        case network::MO_AUTH:
            responseCode = authRR(reqContext, resContext);
            if(responseCode == ResponseCode::status_success)
            {
                saveSession(session, resContext);
            }
            break;
        case network::MO_REGISTER:
            responseCode = registerRR(reqContext, resContext);
            break;
        case network::MO_LOGOUT:
            responseCode = logOutRR(reqContext);
            break;
        case network::MO_USER_PACKAGE_INFO:
            responseCode = packageInfoRR(reqContext, resContext);
            break;
        case network::MO_USER_STORY:
            responseCode = userStoryRR(reqContext, resContext);
            break;
        case network::MO_ALL_RATES:
            responseCode = allRatesRR(reqContext, resContext);
            break;
        case network::MO_ALL_SERVICES:
            responseCode = allServicesRR(reqContext, resContext);
            break;
        case network::MO_RATE_PAYMENT:
            responseCode = ratePaymentRR(reqContext, resContext);
            break;
        case network::MO_SERVICE_PAYMENT:
            responseCode = servicePaymentRR(reqContext, resContext);
            break;
        case network::MO_ALL_USERS:
            responseCode = allUsersRR(reqContext, resContext);
            break;
        case network::MO_ADMIN_UPDATE_USER_PACKAGE:
            responseCode = updateUserPackage(reqContext, resContext);
            break;
        case network::MO_ADMIN_USER_RATE_CHANGE:
            responseCode = adminRateChangeRR(reqContext, resContext);
            break;
        case network::MO_ADMIN_USER_SERVICE_CHANGE:
            responseCode = adminServiceChangeRR(reqContext, resContext);
            break;
        }
    }
    while(false);

    LOG_INFO("Request [" << reqContext.message_type_()
             << "] end with status: "
             << (responseCode == ResponseCode::status_success ? "SUCCESS":"ERROR")
             << " " << static_cast<int>(responseCode));

    resContext.set_error_code(static_cast<int32_t>(responseCode));
    resContext.set_message_type_(reqContext.message_type_());


    session->execute(resContext.SerializeAsString());
}

void RRManager::disconectedSession(std::shared_ptr<ClientChannel> session)
{
    LOG_INFO("Disconected! session: " << session->getSessionId());
}

void RRManager::saveSession(std::shared_ptr<ClientChannel> session, const network::ResponseContext &response)
{
    session->setSessionId(response.session_info().session_id());
    UserInfo userInfo;
    AuthPostgresManager::getUserByLogin(response.session_info().login(), userInfo);
    session->setUserInfo(userInfo);
}

ResponseCode RRManager::checkSession(network::RequestContext &request, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        bool isGood;
        resultStatus = SessionPostgresInfo::checkSession(request.session_info().session_id(), isGood);

        if(resultStatus != ResponseCode::status_success)
        {
            LOG_ERR("Cannot check session!");
            break;
        }

        if(isGood)
        {
            resultStatus = SessionPostgresInfo::updateSession(request.session_info().session_id());
            if(resultStatus != ResponseCode::status_success)
            {
                LOG_ERR("Cannot update session!");
                break;
            }

            uint64_t user_id;
            resultStatus = SessionPostgresInfo::getUserBySession(request.session_info().session_id(), user_id);
            if(resultStatus != ResponseCode::status_success)
            {
                LOG_ERR("Cannot update session!");
                break;
            }

            UserInfo uInfo;
            AuthPostgresManager::getUserByUserId(user_id, uInfo);

            network::SessionInfo* info = new network::SessionInfo();
            info->set_session_id(request.session_info().session_id());
            info->set_login(uInfo.user_login);
            info->set_role(uInfo.role);
            info->set_userid(user_id);

            request.set_allocated_session_info(new network::SessionInfo(*info));
            response.set_allocated_session_info(info);
        }
    }
    while(false);

    return resultStatus;
}


ResponseCode RRManager::authRR(const network::RequestContext &request, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!request.has_auth_message_())
        {
            LOG_ERR("Where is not register message!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        network::AuthMessage authMessage = request.auth_message_();

        if(!authMessage.has_login())
        {
            LOG_ERR("Where is not login value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        if(!authMessage.has_pass())
        {
            LOG_ERR("Where is not password value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }
        network::AuthMessageResponse* regRes = new network::AuthMessageResponse();
        network::SessionInfo* sessionInfo = new network::SessionInfo();
        resultStatus = AuthLogic::authUser(authMessage, regRes, sessionInfo);

        response.set_allocated_auth_response(regRes);
        response.set_allocated_session_info(sessionInfo);

        LOG_INFO("Send response-message to client " << authMessage.login()
                 << " message [" << regRes->server_message() << "]");
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::registerRR(const network::RequestContext &requests, network::ResponseContext& response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_register_message_())
        {
            LOG_ERR("Where is not register message!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        network::RegisterMessage authMessage = requests.register_message_();

        if(!authMessage.has_login())
        {
            LOG_ERR("Where is not login value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        if(!authMessage.has_pass())
        {
            LOG_ERR("Where is not password value!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }
        network::RegisterMessageResponse* regRes = new network::RegisterMessageResponse();

        resultStatus = AuthLogic::createUser(authMessage, regRes, static_cast<PostgresRole>(requests.session_info().role()));

        response.set_allocated_register_response(regRes);

        LOG_INFO("Send response-message to client " << authMessage.login()
                 << " message [" << regRes->messagetext() << "]");
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::logOutRR(const network::RequestContext &requests)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_logout_message())
        {
            LOG_ERR("Where is not logout message!");
            resultStatus = ResponseCode::status_bad_request;
            break;
        }

        resultStatus = AuthLogic::logout(requests);
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::packageInfoRR(const network::RequestContext &requests, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_package_info())
        {
            LOG_ERR("Request dont has package info");
            break;
        }

        network::UserPackageInfoMessage upim = requests.package_info();

        if(!upim.has_user_id())
        {
            LOG_ERR("Request dont has user id");
            break;
        }

        LOG_INFO("Package info for user " << upim.user_id());

        User user;
        resultStatus = AuthLogic::getPackageState(upim.user_id(), user, static_cast<PostgresRole>(requests.session_info().role()));

        if(resultStatus != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting user package info!");
            break;
        }

        network::UserPackageInfoResponse* upr = new network::UserPackageInfoResponse();
        upr->set_phone(user.user_info.user_login);
        upr->set_score(user.user_info.score);
        upr->set_count_mb(user.package.count_mb);
        upr->set_count_sec_into_net(user.package.count_sec_into_net);
        upr->set_count_sec_out_net(user.package.count_sec_out_net);
        upr->set_count_sms(user.package.count_sms);

        network::UserRateInfo* userRate = new network::UserRateInfo();
        network::RateInfo* rateInfo = new network::RateInfo();

        user.rate_info.serialize_to_pb(rateInfo);
        userRate->set_connect_date(user.rate_info.connected_date);
        userRate->set_allocated_rate_info(rateInfo);

        upr->set_allocated_user_rate(userRate);
        network::UserServiceInfo* usi = new network::UserServiceInfo();
        for(ServiceInfoDate sid: user.services)
        {
            auto us = usi->add_user_services();
            us->set_allocated_service(new network::ServiceInfo(sid.first));
            us->set_connected_date(sid.second);
        }
        upr->set_allocated_user_service_info(usi);

        response.set_allocated_package_response(upr);
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::userStoryRR(const network::RequestContext &requests, network::ResponseContext &response)
{

    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_user_story())
        {
            LOG_ERR("Request dont have user story message");
            break;
        }

        network::UserStoryMessage usm = requests.user_story();

        if(!usm.has_user_id())
        {
            LOG_ERR("Request dont have user id");
            break;
        }

        LOG_INFO("User id for user story request " << usm.user_id());

        UserStory story;
        resultStatus = AuthLogic::getUserStory(usm.user_id(), story, static_cast<PostgresRole>(requests.session_info().role()));

        if(resultStatus != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting user story!");
            break;
        }

        network::UserPaidStoryResponse* upsr = new network::UserPaidStoryResponse();

        for(network::UserConnectedRate& rate: story.rates)
        {
            network::UserConnectedRate* ucr = upsr->add_rates();
            *ucr = rate;
        }

        for(network::UserConnectedService& rate: story.services)
        {
            network::UserConnectedService* ucr = upsr->add_services();
            *ucr = rate;
        }

        for(network::SmsPayment& rate: story.sms)
        {
            network::SmsPayment* ucr = upsr->add_sms();
            *ucr = rate;
        }

        for(network::OutboundCallPayment& rate: story.calls)
        {
            network::OutboundCallPayment* ucr = upsr->add_calls();
            *ucr = rate;
        }

        response.set_allocated_story_response(upsr);
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::allServicesRR(const network::RequestContext &requests, network::ResponseContext &response)
{

    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        std::vector<network::ServiceInfo> services;
        resultStatus = AuthLogic::getAllServices(requests.session_info().userid(), services, static_cast<PostgresRole>(requests.session_info().role()));

        if(resultStatus != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting all services!");
            break;
        }
        network::AllServicesResponse* asr = new network::AllServicesResponse();

        for(network::ServiceInfo& si: services)
        {
            network::ServiceInfo* s = asr->add_allservices();
            *s = si;
        }

        response.set_allocated_all_services(asr);
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::allRatesRR(const network::RequestContext &requests, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        std::vector<network::RateInfo> rates;
        resultStatus = AuthLogic::getAllRates(requests.session_info().userid(), rates, static_cast<PostgresRole>(requests.session_info().role()));

        if(resultStatus != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting all rate!");
            break;
        }
        network::AllRatesResponse* asr = new network::AllRatesResponse();

        for(network::RateInfo& si: rates)
        {
            network::RateInfo* s = asr->add_allrates();
            *s = si;
        }

        response.set_allocated_all_rates(asr);
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::ratePaymentRR(const network::RequestContext &requests, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_rate_payment())
        {
            LOG_ERR("Request dont have rate payment message");
            break;
        }

        network::RatePaymentMessage rpm = requests.rate_payment();

        if(!rpm.has_rate_id())
        {
            LOG_ERR("Request dont have rate id");
            break;
        }

        network::RatePaymentResponse* rpr = new network::RatePaymentResponse();
        resultStatus = AuthLogic::ratePayment(requests.session_info().userid(), rpm, *rpr, static_cast<PostgresRole>(requests.session_info().role()));
        response.set_allocated_ratepaymentresponse(rpr);

    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::servicePaymentRR(const network::RequestContext &requests, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_service_payment())
        {
            LOG_ERR("Request dont have rate payment message");
            break;
        }

        network::ServicePaymentMessage rpm = requests.service_payment();

        if(!rpm.has_rate_id())
        {
            LOG_ERR("Request dont have service id");
            break;
        }

        network::ServicePaymentResponse* rpr = new network::ServicePaymentResponse();
        resultStatus = AuthLogic::servicePayment(requests.session_info().userid(), rpm, *rpr, static_cast<PostgresRole>(requests.session_info().role()));
        response.set_allocated_servicepaymentresponse(rpr);

    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::allUsersRR(const network::RequestContext &requests, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        network::AllUsersResponse* rpr = new network::AllUsersResponse();
        std::vector<std::pair<std::string, uint64_t>> users;
        resultStatus = AuthLogic::getAllUsers(users, static_cast<PostgresRole>(requests.session_info().role()));

        for(std::pair<std::string, uint64_t>& pair: users)
        {
            network::AllUsersResponse_PhoneId* pi = rpr->add_phone_id();
            pi->set_phone(pair.first);
            pi->set_user_id(pair.second);
        }

        response.set_allocated_all_users_response(rpr);

    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::updateUserPackage(const network::RequestContext &requests, network::ResponseContext &response)
{

    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_admin_user_package())
        {
            LOG_ERR("Request dont have admin user package message");
            break;
        }

        network::AdminUpdateUserPackage auup = requests.admin_user_package();

        if(!auup.has_count_mb() || !auup.has_count_sec_into_net() || !auup.has_count_sec_out_net()
                || !auup.has_count_sms() || !auup.has_score() || !auup.has_user_id())
        {
            LOG_ERR("AdminUpdateUserPackage dont have one or more of the parametr");
            break;
        }

        network::AdminUpdateUserPackageResponse* auupr = new network::AdminUpdateUserPackageResponse();
        resultStatus = AuthLogic::adminUpdateUserPackage(auup, *auupr);

        response.set_allocated_admin_update_user_package(auupr);
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::adminRateChangeRR(const network::RequestContext &requests, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_admin_user_rate_change())
        {
            LOG_ERR("Request dont have admin user rate change message");
            break;
        }

        network::AdminUserRateChange aurp = requests.admin_user_rate_change();

        if(!aurp.has_rate_id() || !aurp.has_user_id())
        {
            LOG_ERR("AdminUserRateChange dont have one or more of the parametr");
            break;
        }

        network::AdminUserRateChangeResponse* auupr = new network::AdminUserRateChangeResponse();
        resultStatus = AuthLogic::adminUserRateChange(aurp, *auupr);

        response.set_allocated_admin_user_rate_change(auupr);
    }
    while(false);

    return resultStatus;
}

ResponseCode RRManager::adminServiceChangeRR(const network::RequestContext &requests, network::ResponseContext &response)
{
    ResponseCode resultStatus = ResponseCode::status_internal_error;

    do
    {
        if(!requests.has_admin_user_service_change())
        {
            LOG_ERR("Request dont have admin user service change message");
            break;
        }

        network::AdminUserServiceChange aurp = requests.admin_user_service_change();

        if(!aurp.has_service_id() || !aurp.has_user_id())
        {
            LOG_ERR("AdminUserServiceChange dont have one or more of the parametr");
            break;
        }

        network::AdminUserServiceChangeResponse* auupr = new network::AdminUserServiceChangeResponse();
        resultStatus = AuthLogic::adminUserServiceChange(aurp, *auupr);

        response.set_allocated_admin_user_service(auupr);
    }
    while(false);

    return resultStatus;
}
