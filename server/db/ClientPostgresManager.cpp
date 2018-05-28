#include "ClientPostgresManager.h"

ResponseCode ClientPostgresManager::getUserInfo(uint64_t id, UserInfo &uInfo)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getClientConnection();

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("getUser"))
            {
                connection->prepare("getUserByLogin",
                                   "SELECT id, score, phone "
                                   "FROM users "
                                   "WHERE id = $1;");
            }

            pqxx::work work(*connection, "getUserByLogin");

            pqxx::result query_result = work.prepared("getUserByLogin")(id).exec();
            if(!query_result.empty())
            {
                uInfo.parse_from_mo_db(query_result[0]);
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

ResponseCode ClientPostgresManager::createUser(const UserInfo &uInfo)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getClientConnection();

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("getUser"))
            {
                connection->prepare("createUser",
                                   "INSERT INTO users(id, score, phone) "
                                   " VALUES($1, 0.0, $2::varchar);");
            }

            pqxx::work work(*connection, "createUser");

            work.prepared("createUser")(uInfo.user_id)(uInfo.user_login).exec();

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query createUser err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}
