#include "ClientPostgresManager.h"

ResponseCode ClientPostgresManager::getUserInfo(uint64_t id, UserInfo &uInfo, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getDBConnection(role);

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

ResponseCode ClientPostgresManager::createUser(const UserInfo &uInfo, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getDBConnection(role);

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

ResponseCode ClientPostgresManager::updateUser(UserInfo &info, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getDBConnection(role);

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("updateUser"))
            {
                connection->prepare("updateUser",
                                   "UPDATE users SET score = $1 "
                                   " WHERE id = $2;");
            }

            pqxx::work work(*connection, "updateUser");

            work.prepared("updateUser")(info.score)(info.user_id).exec();

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

ResponseCode ClientPostgresManager::getAllUsers(std::vector<PhoneUserId> &users, PostgresRole role)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getDBConnection(role);

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("getAllUsers"))
            {
                connection->prepare("getAllUsers",
                                   "SELECT id, phone FROM users;");
            }

            pqxx::work work(*connection, "getAllUsers");

            pqxx::result query_result = work.prepared("getAllUsers").exec();

            for(const pqxx::tuple& value: query_result)
            {
                users.push_back(std::make_pair(value["phone"].as<std::string>()
                                                , value["id"].as<uint64_t>()));
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getAllUsers err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}
