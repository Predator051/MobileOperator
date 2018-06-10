#include "RatePostgresManager.h"
#include "Helper.h"

ResponseCode RatePostgresManager::getRateInfo(uint64_t id, RateInfo &rate, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getRateInfo"))
            {
                connection->prepare("getRateInfo",
                                   "SELECT id, into_net_price, out_net_price, "
                                   "sms_price, count_mb, count_sms, count_sec_into_net, "
                                   "count_sec_out_net, period, about, cost "
                                   "FROM rates WHERE id = $1;");
            }

            pqxx::work work(*connection, "getRateInfo");

            pqxx::result res = work.prepared("getRateInfo")(id).exec();

            if(res.empty())
            {
                result = ResponseCode::status_does_not_exist;
                break;
            }

            rate.parse_from_pg(res[0]);

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getRateInfo err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode RatePostgresManager::getUserRate(uint64_t user_id, uint64_t &rate_id, uint64_t& connected_date, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getUserRate"))
            {
                connection->prepare("getUserRate",
                                    "SELECT MAX(date) as cdate, rate_id "
                                    "FROM user_connected_rate "
                                    "WHERE user_id = $1 "
                                    "GROUP BY rate_id, user_id "
                                    "ORDER BY cdate DESC;");
            }

            pqxx::work work(*connection, "getUserRate");

            pqxx::result res = work.prepared("getUserRate")(user_id).exec();

            if(res.empty())
            {
                result = ResponseCode::status_does_not_exist;
                break;
            }

            rate_id = res[0]["rate_id"].as<uint64_t>();
            connected_date = Helper::strDateToInt(res[0]["cdate"].as<std::string>());
            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getUserRate err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode RatePostgresManager::getUserConnectedRate(uint64_t user_id, std::vector<RateDate>& rates, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getUserConnectedRate"))
            {
                connection->prepare("getUserConnectedRate",
                                    "SELECT rate_id, date "
                                    "FROM user_connected_rate "
                                    "WHERE user_id = $1 "
                                    "AND date > NOW() - '3 month'::interval;");
            }

            pqxx::work work(*connection, "getUserConnectedRate");

            pqxx::result res = work.prepared("getUserConnectedRate")(user_id).exec();

            for(const pqxx::tuple& value: res)
            {
                uint64_t rate_id = value["rate_id"].as<uint64_t>();
                uint64_t connected_date = Helper::strDateToInt(value["date"].as<std::string>());
                rates.push_back(std::make_pair(rate_id, connected_date));
            }
            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getUserRate err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode RatePostgresManager::getAllRates(std::vector<RateInfo> &rates, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getAllRates"))
            {
                connection->prepare("getAllRates",
                                   "SELECT id, into_net_price, out_net_price, "
                                   "sms_price, count_mb, count_sms, count_sec_into_net, "
                                   "count_sec_out_net, period, about, cost "
                                   "FROM rates;");
            }

            pqxx::work work(*connection, "getAllRates");

            pqxx::result res = work.prepared("getAllRates").exec();

            for(const pqxx::tuple& value: res)
            {
                RateInfo ri;
                ri.parse_from_pg(value);
                rates.push_back(ri);
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getRateInfo err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode RatePostgresManager::createUserRate(uint64_t user_id, uint64_t rate_id, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("createUserRate"))
            {
                connection->prepare("createUserRate",
                                   "INSERT INTO user_connected_rate (user_id, rate_id, date) "
                                   "VALUES ($1, $2, NOW());");
            }

            pqxx::work work(*connection, "createUserRate");

            work.prepared("createUserRate")(user_id)(rate_id).exec();

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getRateInfo err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}
