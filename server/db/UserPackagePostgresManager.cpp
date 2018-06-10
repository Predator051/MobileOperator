#include "UserPackagePostgresManager.h"
#include "ProtoHelper.h"

ResponseCode UserPackagePostgresManager::getPackage(PackageInfo &packageInfo, uint32_t user_id, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getPackage"))
            {
                connection->prepare("getPackage",
                                   "SELECT count_sec_into_net, count_sec_out_net, count_sms, count_mb "
                                   "FROM package_state "
                                   "WHERE user_id = $1;");
            }

            pqxx::work work(*connection, "getPackage");

            pqxx::result res = work.prepared("getPackage")(user_id).exec();

            if(res.empty())
            {
                result = ResponseCode::status_does_not_exist;
                break;
            }

            packageInfo.parse_from_pg(res[0]);

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getPackage err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode UserPackagePostgresManager::getSmsPayment(uint64_t user_id, std::vector<network::SmsPayment> &sms, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getPackage"))
            {
                connection->prepare("getPackage",
                                   "SELECT user_id, paid, date "
                                   "FROM sms_payment "
                                   "WHERE user_id = $1;");
            }

            pqxx::work work(*connection, "getPackage");

            pqxx::result res = work.prepared("getPackage")(user_id).exec();

            for(const pqxx::tuple& value: res)
            {
                network::SmsPayment s;
                ProtoHelper::parse_from_pg(value, s);
                sms.push_back(s);
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getPackage err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode UserPackagePostgresManager::getOutboundCalls(uint64_t user_id, std::vector<network::OutboundCallPayment> &out, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getPackage"))
            {
                connection->prepare("getPackage",
                                   "SELECT user_id_from, number_to, date, paid, count_sec "
                                   "FROM outbound_call_payment "
                                   "WHERE user_id_from = $1;");
            }

            pqxx::work work(*connection, "getPackage");

            pqxx::result res = work.prepared("getPackage")(user_id).exec();

            for(const pqxx::tuple& value: res)
            {
                network::OutboundCallPayment s;
                ProtoHelper::parse_from_pg(value, s);
                out.push_back(s);
            }

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getPackage err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode UserPackagePostgresManager::updatePackage(PackageInfo &packageInfo, uint32_t user_id, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getPackage"))
            {
                connection->prepare("getPackage",
                                   "UPDATE package_state "
                                   "SET count_sec_into_net = $1, count_sec_out_net = $2"
                                   ", count_sms = $3, count_mb = $4 "
                                   "WHERE user_id = $5;");
            }

            pqxx::work work(*connection, "getPackage");

            work.prepared("getPackage")
                    (packageInfo.count_sec_into_net)
                    (packageInfo.count_sec_out_net)
                    (packageInfo.count_sms)
                    (packageInfo.count_mb)
                    (user_id).exec();

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getPackage err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode UserPackagePostgresManager::updatePackageAdmin(PackageInfo &packageInfo, uint32_t user_id)
{
    ResponseCode result = ResponseCode::status_internal_error;

    do
    {
        try
        {
            db_connection_ptr connection = DBHelper::getDBConnection(PostgresRole::ADMIN);

            if(!connection)
            {
                LOG_ERR("Cannot create connection to auth bd!");
                break;
            }

            if(!DBHelper::getDBHelper().isPrepared("updatePackageAdmin"))
            {
                connection->prepare("updatePackageAdmin",
                                   "UPDATE package_state "
                                   "SET count_sec_into_net = $1, count_sec_out_net = $2"
                                   ", count_sms = $3, count_mb = $4 "
                                   "WHERE user_id = $5;");
            }

            pqxx::work work(*connection, "updatePackageAdmin");

            work.prepared("updatePackageAdmin")
                    (packageInfo.count_sec_into_net)
                    (packageInfo.count_sec_out_net)
                    (packageInfo.count_sms)
                    (packageInfo.count_mb)
                    (user_id).exec();

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query getPackage err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}

ResponseCode UserPackagePostgresManager::createEmptyPackage(uint32_t user_id, PostgresRole role)
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

            if(!DBHelper::getDBHelper().isPrepared("getPackage"))
            {
                connection->prepare("createEmptyPackage",
                                   "INSERT INTO package_state (count_sec_into_net, count_sec_out_net"
                                   ", count_sms, count_mb, user_id) "
                                   "VALUES (0,0,0,0, $1);");
            }

            pqxx::work work(*connection, "createEmptyPackage");

            work.prepared("createEmptyPackage")
                    (user_id).exec();

            work.commit();
            result = ResponseCode::status_success;
        }
        catch(const std::exception& e)
        {
            LOG_ERR("Failure: trying to query createEmptyPackage err: " << e.what());
            break;
        }
    }
    while(false);

    return result;
}
