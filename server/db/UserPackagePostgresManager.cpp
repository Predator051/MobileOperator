#include "UserPackagePostgresManager.h"

ResponseCode UserPackagePostgresManager::getPackage(PackageInfo &packageInfo, uint32_t user_id)
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
