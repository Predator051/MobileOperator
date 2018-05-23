#include "AuthLogic.h"
#include "db/AuthPostgresManager.h"
#include "Config/GlobalParams.h"
#include "Crypto/CryptoHelper.h"
#include "db/SessionPostgresInfo.h"

ResponseCode AuthLogic::createUser(const network::RegisterMessage &authData, network::RegisterMessageResponse* response)
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
        bool isSessionExist = false;
        result = SessionPostgresInfo::isSessionExist(userInfo.user_id, isSessionExist);

        if(isSessionExist)
        {
            LOG_INFO("User " << userInfo.user_login << " already login!");
            response->set_server_message("This user already login");
            response->set_status(false);
            result = ResponseCode::status_internal_error;
            break;
        }

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

ResponseCode AuthLogic::logout(const network::LogOutMessage &logoutData)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        std::string login = logoutData.login();
        UserInfo uInfo;
        AuthPostgresManager::getUserByLogin(login, uInfo);

        result = SessionPostgresInfo::removeSession(uInfo.user_id);
    }
    while(false);

    return result;
}
