#include "RatePostgresManager.h"
#include "Helper.h"

ResponseCode RatePostgresManager::getRateInfo(uint64_t id, RateInfo &rate)
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

ResponseCode RatePostgresManager::getUserRate(uint64_t user_id, uint64_t &rate_id, uint64_t& connected_date)
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

            if(!DBHelper::getDBHelper().isPrepared("getUserRate"))
            {
                connection->prepare("getUserRate",
                                    "SELECT DISTINCT ON (user_id) MAX(date) as cdate, rate_id "
                                    "FROM user_connected_rate "
                                    "WHERE user_id = $1 "
                                    "GROUP BY rate_id, user_id;");
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
