#include "AuthLogic.h"
#include "db/AuthPostgresManager.h"
#include "Config/GlobalParams.h"
#include "Crypto/CryptoHelper.h"

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
            if(!response)
            {
                response->set_messagetext("Something wrong!");
                response->set_status(false);
            }
            break;
        }

        if(isExist)
        {
            LOG_ERR("User already exist!");
            if(!response)
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
            if(!response)
            {
                response->set_messagetext("Cannot create user! Try again");
                response->set_status(false);
            }
        }
        else
        {
            if(!response)
            {
                response->set_messagetext("Success");
                response->set_status(true);
            }
        }
    }
    while(false);

    return result;
}
