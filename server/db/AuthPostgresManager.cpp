#include "AuthPostgresManager.h"

ResponseCode AuthPostgresManager::isUserExist(const std::string &login, bool &exist)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getAuthConnection();

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("IsUserExist"))
            {
                connection->prepare("IsUserExist",
                                   "SELECT COUNT(*) FROM server_users WHERE user_login = $1::varchar");
            }

            pqxx::work work(*connection, "IsUserExist");

            pqxx::result query_result = work.prepared("IsUserExist")(login).exec();

            exist = query_result[0][0].as<int>() > 0;

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query IsUserExist err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode AuthPostgresManager::createUser(const std::string &login, const std::string pass, const std::string& salt)
{

    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getAuthConnection();

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("CreateUser"))
            {
                connection->prepare("CreateUser",
                                   "INSERT INTO server_users(user_id, user_login, password, creation_time, role, salt) "
                                   "VALUES (DEFAULT, $1::varchar, $2::varchar, DEFAULT, 0, $3::varchar);");
            }

            pqxx::work work(*connection, "CreateUser");

            work.prepared("CreateUser")(login)(pass)(salt).exec();

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query RememberMailChain err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode AuthPostgresManager::getUserByLogin(const std::string &login, UserInfo &userInfo)
{

    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getAuthConnection();

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("getUserByLogin"))
            {
                connection->prepare("getUserByLogin",
                                   "SELECT user_id, user_login, password, creation_time, role, salt "
                                   "FROM server_users "
                                   "WHERE user_login = $1::varchar;");
            }

            pqxx::work work(*connection, "getUserByLogin");

            pqxx::result query_result = work.prepared("getUserByLogin")(login).exec();
            if(!query_result.empty())
            {
                userInfo.parse_from_pg(query_result[0]);
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getUserByLogin err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}
