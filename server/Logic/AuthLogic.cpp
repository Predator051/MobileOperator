#include "AuthLogic.h"
#include "db/AuthPostgresManager.h"
#include "Config/GlobalParams.h"
#include "Crypto/CryptoHelper.h"
#include "db/SessionPostgresInfo.h"
#include "db/ClientPostgresManager.h"
#include "db/UserPackagePostgresManager.h"
#include "db/RatePostgresManager.h"
#include "db/ServiceManagerPostgres.h"

ResponseCode AuthLogic::createUser(const network::RegisterMessage &authData, network::RegisterMessageResponse* response, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        bool isExist = false;
        result = AuthPostgresManager::isUserExist(authData.login(), isExist);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Error checking if user exist!");
            if(response)
            {
                response->set_messagetext("Something wrong!");
                response->set_status(false);
            }
            break;
        }

        if(isExist)
        {
            LOG_ERR("User already exist!");
            if(response)
            {
                response->set_messagetext("Same user already exist!");
                response->set_status(false);
            }
            break;
        }

        std::string passWithSalt = authData.pass() + GlobalsParams::getPostgres_default_solt();
        std::string uniqSalt = CryptoHelper::gen_random_string(30);
        std::string hashedPass;
        CryptoHelper::md5_hash(passWithSalt + uniqSalt, hashedPass);

        if(AuthPostgresManager::createUser(authData.login(), hashedPass, uniqSalt) != ResponseCode::status_success)
        {
            if(response)
            {
                response->set_messagetext("Cannot create user! Try again");
                response->set_status(false);
            }
        }
        else
        {
            UserInfo uInfo;
            AuthPostgresManager::getUserByLogin(authData.login(), uInfo);
            result = ClientPostgresManager::createUser(uInfo, role);
            if(result != ResponseCode::status_success)
            {
                if(response)
                {
                    response->set_messagetext("Cannot create user! Try again");
                    response->set_status(false);
                }
            }

            result = UserPackagePostgresManager::createEmptyPackage(uInfo.user_id, role);
            result = RatePostgresManager::createUserRate(uInfo.user_id, 3, role);
            if(response)
            {
                response->set_messagetext("Success");
                response->set_status(true);
            }
        }
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::authUser(const network::AuthMessage &authData, network::AuthMessageResponse *response, network::SessionInfo* sessionInfo)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        bool isExist = false;
        result = AuthPostgresManager::isUserExist(authData.login(), isExist);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Error checking if user exist!");
            if(response)
            {
                response->set_server_message("Something wrong!");
                response->set_status(false);
            }
            break;
        }

        if(!isExist)
        {
            LOG_ERR("User does not exist!");
            if(response)
            {
                response->set_server_message("Same user does not exist!");
                response->set_status(false);
            }
            break;
        }

        UserInfo userInfo;
        result = AuthPostgresManager::getUserByLogin(authData.login(), userInfo);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure getting userInfo by login " << authData.login());
            break;
        }

        std::string session;

        std::string passWithSalt = authData.pass() + GlobalsParams::getPostgres_default_solt();
        std::string uniqSalt = userInfo.salt;
        std::string hashedPass;
        CryptoHelper::md5_hash(passWithSalt + uniqSalt, hashedPass);

        if(hashedPass == userInfo.password)
        {
            LOG_INFO("User " << userInfo.user_login
                     << " was entered correct password");
            response->set_server_message("Welcome!");
            response->set_status(true);
            session = CryptoHelper::gen_random_string(25);
            SessionPostgresInfo::createSession(userInfo.user_id, session);
            sessionInfo->set_session_id(session);
            sessionInfo->set_login(userInfo.user_login);
            sessionInfo->set_userid(userInfo.user_id);
            sessionInfo->set_role(userInfo.role);
        }
        else
        {
            LOG_INFO("User " << userInfo.user_login
                     << " was entered incorrect password");
            response->set_server_message("Incorrect password or login!");
            response->set_status(false);
        }
    }
    while(false);

    return result;

}

ResponseCode AuthLogic::logout(const network::RequestContext &request)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        auto session = request.session_info();

        result = SessionPostgresInfo::updateSessionToNow(session.session_id());
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::getPackageState(uint64_t user_id, User &user, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        PackageInfo package;
        result = UserPackagePostgresManager::getPackage(package, user_id, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting package for user " << user_id);
            break;
        }

        UserInfo uInfo;
        result = ClientPostgresManager::getUserInfo(user_id, uInfo, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting user info by " << user_id);
            break;
        }

        RateInfo rInfo;
        uint64_t con_date;
        uint64_t rId;
        result = RatePostgresManager::getUserRate(user_id, rId, con_date, role);
        rInfo.id = rId;
        rInfo.connected_date = con_date;
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting rate id for user " << user_id);
            break;
        }

        result = RatePostgresManager::getRateInfo(rInfo.id, rInfo, role);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting rate info for user " << user_id);
            break;
        }

        std::vector<ServiceDate> services_id;
        result = ServiceManagerPostgres::getUserServices(user_id, services_id, role);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting services id for user " << user_id);
            break;
        }

        for(ServiceDate id: services_id)
        {
            network::ServiceInfo info;
            ServiceManagerPostgres::getServiceInfo(id.first, info, role);
            user.services.push_back(std::make_pair(info, id.second));
        }

        user.package = package;
        user.user_info = uInfo;
        user.rate_info = rInfo;
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::getUserStory(uint64_t user_id, UserStory &story, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        std::vector<RateDate> userRatesDate;
        result = RatePostgresManager::getUserConnectedRate(user_id, userRatesDate, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to rates for user " << user_id);
            break;
        }

        for(RateDate& rd: userRatesDate)
        {
            RateInfo rate;
            RatePostgresManager::getRateInfo(rd.first, rate, role);
            network::RateInfo* rateInfo = new network::RateInfo();
            rate.serialize_to_pb(rateInfo);
            network::UserConnectedRate ucr;
            ucr.set_allocated_rate(rateInfo);
            ucr.set_user_id(user_id);
            ucr.set_date(rd.second);
            story.rates.push_back(ucr);
        }

        std::vector<ServiceDate> userServicesDate;
        result = ServiceManagerPostgres::getUserConnectedService(user_id, userServicesDate, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting services for user " << user_id);
            break;
        }

        for(ServiceDate& rd: userServicesDate)
        {
            network::ServiceInfo* sInfo = new network::ServiceInfo();
            ServiceManagerPostgres::getServiceInfo(rd.first, *sInfo, role);
            network::UserConnectedService ucs;
            ucs.set_allocated_service(sInfo);
            ucs.set_date(rd.second);
            ucs.set_user_id(user_id);
            story.services.push_back(ucs);
        }

        result = UserPackagePostgresManager::getOutboundCalls(user_id, story.calls, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting call for user " << user_id);
            break;
        }

        result = UserPackagePostgresManager::getSmsPayment(user_id, story.sms, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to getting sms for user " << user_id);
            break;
        }
    }
    while(false);

    return result;

}

ResponseCode AuthLogic::getAllRates(uint64_t user_id, std::vector<network::RateInfo>& rates, PostgresRole role)
{

    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        std::vector<RateInfo> rs;
        result = RatePostgresManager::getAllRates(rs, role);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to all rates " << user_id);
            break;
        }

        for(RateInfo& ri: rs)
        {
            network::RateInfo nri;
            ri.serialize_to_pb(&nri);
            rates.push_back(nri);
        }
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::getAllServices(uint64_t user_id, std::vector<network::ServiceInfo>& services, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        result = ServiceManagerPostgres::getAllServices(services, role);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to all services " << user_id);
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::ratePayment(uint64_t user_id, network::RatePaymentMessage &rpm, network::RatePaymentResponse &response, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        RateInfo rInfo;
        result = RatePostgresManager::getRateInfo(rpm.rate_id(), rInfo, role);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get rate info" << user_id);
            response.set_message_res("Maybe this rate already does not exist");
            break;
        }

        UserInfo uInfo;
        result = ClientPostgresManager::getUserInfo(user_id, uInfo, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get user info" << user_id);
            response.set_message_res("Something wrong!");
            break;
        }

        if(uInfo.score < rInfo.cost)
        {
            LOG_INFO("User dont has enough many " << uInfo.score << ":" << rInfo.cost);
            response.set_message_res("You dont have enough many!");
            break;
        }

        result = RatePostgresManager::createUserRate(user_id, rInfo.id, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to create connection user " << user_id << " with rate " << rInfo.id);
            response.set_message_res("Something wrong!");
            break;
        }

        uInfo.score -= rInfo.cost;
        ClientPostgresManager::updateUser(uInfo, role);

        PackageInfo pInfo;
        result = UserPackagePostgresManager::getPackage(pInfo, user_id, role);

        pInfo.count_mb += rInfo.count_mb;
        pInfo.count_sms += rInfo.count_sms;
        pInfo.count_sec_into_net += rInfo.count_sec_into_net;
        pInfo.count_sec_out_net += rInfo.count_sec_out_net;

        result = UserPackagePostgresManager::updatePackage(pInfo, user_id, role);

        if(result == ResponseCode::status_success)
        {
            response.set_message_res("Success!");
        }
        else
        {
            response.set_message_res("Something wrong!");
        }
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::servicePayment(uint64_t user_id, network::ServicePaymentMessage &rpm, network::ServicePaymentResponse &response, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        network::ServiceInfo sInfo;
        result = ServiceManagerPostgres::getServiceInfo(rpm.rate_id(), sInfo, role);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get service info" << user_id);
            response.set_message_res("Maybe this rate already does not exist");
            break;
        }

        UserInfo uInfo;
        result = ClientPostgresManager::getUserInfo(user_id, uInfo, role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get user info" << user_id);
            response.set_message_res("Something wrong!");
            break;
        }

        if(uInfo.score < sInfo.cost())
        {
            LOG_INFO("User dont has enough many " << uInfo.score << ":" << sInfo.cost());
            response.set_message_res("You dont have enough many!");
            break;
        }

        result = ServiceManagerPostgres::createUserService(user_id, sInfo.id(), role);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to create connection user " << user_id << " with service " << sInfo.id());
            response.set_message_res("Something wrong!");
            break;
        }

        uInfo.score -= sInfo.cost();
        ClientPostgresManager::updateUser(uInfo, role);

        PackageInfo pInfo;
        result = UserPackagePostgresManager::getPackage(pInfo, user_id, role);

        pInfo.count_mb += sInfo.count_mb();
        pInfo.count_sms += sInfo.count_sms();
        pInfo.count_sec_into_net += sInfo.count_sec_into_net();
        pInfo.count_sec_out_net += sInfo.count_sec_out_net();

        result = UserPackagePostgresManager::updatePackage(pInfo, user_id, role);

        if(result == ResponseCode::status_success)
        {
            response.set_message_res("Success!");
        }
        else
        {
            response.set_message_res("Something wrong!");
        }
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::getAllUsers(std::vector<std::pair<std::string, uint64_t> >& result, PostgresRole role)
{
    ResponseCode resultCode = ResponseCode::status_internal_error;

    do
    {
        resultCode = ClientPostgresManager::getAllUsers(result, role);

        if(resultCode != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get all users");
            break;
        }
    }
    while(false);

    return resultCode;
}

ResponseCode AuthLogic::adminUpdateUserPackage(const network::AdminUpdateUserPackage &auup, network::AdminUpdateUserPackageResponse &res)
{
    ResponseCode resultCode = ResponseCode::status_internal_error;

    do
    {
        PackageInfo packInfo;
        packInfo.count_mb = auup.count_mb();
        packInfo.count_sec_into_net = auup.count_sec_into_net();
        packInfo.count_sec_out_net = auup.count_sec_out_net();
        packInfo.count_sms = auup.count_sms();

        UserInfo uInfo;
        resultCode = ClientPostgresManager::getUserInfo(auup.user_id(), uInfo, PostgresRole::ADMIN);
        if(resultCode != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get user info" << auup.user_id());
            res.set_status_message("Something wrong!");
            break;
        }

        uInfo.score = auup.score();
        resultCode = ClientPostgresManager::updateUser(uInfo, PostgresRole::ADMIN);
        if(resultCode != ResponseCode::status_success)
        {
            LOG_ERR("Failure to update user info" << auup.user_id());
            res.set_status_message("Something wrong!");
            break;
        }


        resultCode = UserPackagePostgresManager::updatePackageAdmin(packInfo, auup.user_id());
        if(resultCode != ResponseCode::status_success)
        {
            res.set_status_message("Cannot update user package");
            break;
        }
        else
        {
            res.set_status_message("Success");
            break;
        }
    }
    while(false);

    return resultCode;
}

ResponseCode AuthLogic::adminUserRateChange(const network::AdminUserRateChange &rpm, network::AdminUserRateChangeResponse &response)
{

    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        RateInfo rInfo;
        uint64_t user_id = rpm.user_id();
        result = RatePostgresManager::getRateInfo(rpm.rate_id(), rInfo, PostgresRole::ADMIN);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get rate info" << user_id);
            response.set_status_message("Maybe this rate already does not exist");
            break;
        }

        UserInfo uInfo;
        result = ClientPostgresManager::getUserInfo(user_id, uInfo, PostgresRole::ADMIN);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get user info" << user_id);
            response.set_status_message("Something wrong!");
            break;
        }

        result = RatePostgresManager::createUserRate(user_id, rInfo.id, PostgresRole::ADMIN);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to create connection user " << user_id << " with rate " << rInfo.id);
            response.set_status_message("Something wrong!");
            break;
        }

        PackageInfo pInfo;
        result = UserPackagePostgresManager::getPackage(pInfo, user_id, PostgresRole::ADMIN);

        pInfo.count_mb += rInfo.count_mb;
        pInfo.count_sms += rInfo.count_sms;
        pInfo.count_sec_into_net += rInfo.count_sec_into_net;
        pInfo.count_sec_out_net += rInfo.count_sec_out_net;

        result = UserPackagePostgresManager::updatePackage(pInfo, user_id, PostgresRole::ADMIN);

        if(result == ResponseCode::status_success)
        {
            response.set_status_message("Success!");
        }
        else
        {
            response.set_status_message("Something wrong!");
        }
    }
    while(false);

    return result;
}

ResponseCode AuthLogic::adminUserServiceChange(const network::AdminUserServiceChange &rpm, network::AdminUserServiceChangeResponse &response)
{

    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        network::ServiceInfo sInfo;
        uint64_t user_id = rpm.user_id();
        result = ServiceManagerPostgres::getServiceInfo(rpm.service_id(), sInfo, PostgresRole::ADMIN);

        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to get service info" << user_id);
            response.set_status_message("Maybe this rate already does not exist");
            break;
        }

        result = ServiceManagerPostgres::createUserService(user_id, sInfo.id(), PostgresRole::ADMIN);
        if(result != ResponseCode::status_success)
        {
            LOG_ERR("Failure to create connection user " << user_id << " with service " << sInfo.id());
            response.set_status_message("Something wrong!");
            break;
        }

        PackageInfo pInfo;
        result = UserPackagePostgresManager::getPackage(pInfo, user_id, PostgresRole::ADMIN);

        pInfo.count_mb += sInfo.count_mb();
        pInfo.count_sms += sInfo.count_sms();
        pInfo.count_sec_into_net += sInfo.count_sec_into_net();
        pInfo.count_sec_out_net += sInfo.count_sec_out_net();

        result = UserPackagePostgresManager::updatePackage(pInfo, user_id, PostgresRole::ADMIN);

        if(result == ResponseCode::status_success)
        {
            response.set_status_message("Success!");
        }
        else
        {
            response.set_status_message("Something wrong!");
        }
    }
    while(false);

    return result;
}
