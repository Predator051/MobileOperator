#include "ServiceManagerPostgres.h"
#include "Helper.h"

ResponseCode ServiceManagerPostgres::getServiceInfo(int64_t id, network::ServiceInfo &serviceInfo, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getServiceInfo"))
            {
                connection->prepare("getServiceInfo",
                                   "SELECT id, count_mb, count_sms, count_sec_into_net, "
                                   "count_sec_out_net, live_time, about, cost, name "
                                   "FROM services WHERE id = $1;");
            }

            pqxx::work work(*connection, "getServiceInfo");

            pqxx::result res = work.prepared("getServiceInfo")(id).exec();

            if(res.empty())
            {
                result = ResponseCode::status_does_not_exist;
                break;
            }

            ProtoHelper::parse_from_pg(res[0], serviceInfo);

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getServiceInfo err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode ServiceManagerPostgres::getUserServices(int64_t id, std::vector<ServiceDate> &services, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getUserServices"))
            {
                connection->prepare("getUserServices",
                                   "SELECT id, date "
                                    "FROM services AS ser "
                                    "INNER JOIN user_connected_service AS ucs "
                                    "ON ser.id = ucs.service_id "
                                    "WHERE user_id = $1 "
                                    "AND NOW() BETWEEN ucs.date "
                                    "AND ucs.date + (to_char(ser.live_time, '999')||' days')::interval;");
            }

            pqxx::work work(*connection, "getUserServices");

            pqxx::result res = work.prepared("getUserServices")(id).exec();

            for(const pqxx::tuple& id: res)
            {

                services.push_back(std::make_pair(id["id"].as<int64_t>()
                                                  , Helper::strDateToInt(id["date"].as<std::string>())));
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getUserServices err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode ServiceManagerPostgres::getUserConnectedService(int64_t id, std::vector<ServiceDate> &services, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getUserConnectedService"))
            {
                connection->prepare("getUserConnectedService",
                                   "SELECT service_id, date "
                                   "FROM user_connected_service "
                                   "WHERE user_id = $1 AND date > NOW() - '3 month'::interval;");
            }

            pqxx::work work(*connection, "getUserConnectedService");

            pqxx::result res = work.prepared("getUserConnectedService")(id).exec();

            for(const pqxx::tuple& id: res)
            {
                services.push_back(std::make_pair(id["service_id"].as<int64_t>()
                                                  , Helper::strDateToInt(id["date"].as<std::string>())));
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getUserConnectedService err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode ServiceManagerPostgres::getAllServices(std::vector<network::ServiceInfo> &services, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getAllServices"))
            {
                connection->prepare("getAllServices",
                                   "SELECT id, count_mb, count_sms, count_sec_into_net, "
                                   "count_sec_out_net, live_time, about, cost, name "
                                   "FROM services;");
            }

            pqxx::work work(*connection, "getAllServices");

            pqxx::result res = work.prepared("getAllServices").exec();

            for(const pqxx::tuple& value: res)
            {
                network::ServiceInfo serviceInfo;
                ProtoHelper::parse_from_pg(value, serviceInfo);
                services.push_back(serviceInfo);
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getServiceInfo err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode ServiceManagerPostgres::createUserService(int64_t user_id, int64_t service_id, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("createUserService"))
            {
                connection->prepare("createUserService",
                                   "INSERT INTO user_connected_service(user_id, service_id, date) "
                                   "VALUES ($1, $2, NOW());");
            }

            pqxx::work work(*connection, "createUserService");

            work.prepared("createUserService")(user_id)(service_id).exec();

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query createUserService err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}
