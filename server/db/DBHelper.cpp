#include "DBHelper.h"
#include "iostream"
#include "Config/GlobalParams.h"
#include <sstream>
#include <mutex>
#include <algorithm>

db_connection_ptr DBHelper::getDBConnection(PostgresRole role)
{
    switch (role) {
    case PostgresRole::ADMIN:
        return getAdminConnection();
        break;
    case PostgresRole::AUTH:
        return getAuthConnection();
        break;
    case PostgresRole::CLIENT:
        return getClientConnection();
        break;
    }
}

db_connection_ptr DBHelper::getClientConnection()
{
    postgres_role client = GlobalsParams::getClientRole();

    return makeConnection(GlobalsParams::getPostgres_host(), GlobalsParams::getPostgres_port(),
                            client.username, client.password, client.dbname);
}

db_connection_ptr DBHelper::getAdminConnection()
{
    postgres_role admin = GlobalsParams::getAdminRole();

    return makeConnection(GlobalsParams::getPostgres_host(), GlobalsParams::getPostgres_port(),
                            admin.username, admin.password, admin.dbname);

}

db_connection_ptr DBHelper::getAuthConnection()
{
    postgres_role auth = GlobalsParams::getAuthRole();

    return makeConnection(GlobalsParams::getPostgres_host(), GlobalsParams::getPostgres_port(),
                            auth.username, auth.password, auth.dbname);

}

DBHelper &DBHelper::getDBHelper()
{
    static DBHelper helper;
    return helper;
}

void DBHelper::prepareStatetment(const std::string &str)
{
    std::lock_guard<std::mutex> guard(mutex, std::adopt_lock);

    prepared_statetment.push_back(str);
}

bool DBHelper::isPrepared(const std::string &str)
{
    std::lock_guard<std::mutex> guard(mutex, std::adopt_lock);

    std::vector<std::string>::iterator it = std::find_if(prepared_statetment.begin(), prepared_statetment.end(), [str](const std::string& value)
    {
        return str.compare(value) == 0;
    });

    return it!=prepared_statetment.end();
}

db_connection_ptr DBHelper::makeConnection(const std::string &host, const std::string &port, const std::string &user, const std::string &user_pass, const std::string &db)
{
    std::ostringstream conn_string("");
    conn_string << " host="  << host
                << " port="  << port
                << " user=" << user
                << " password=" << user_pass
                << " dbname=" << db;
    return std::make_shared<pqxx::connection>(conn_string.str());
}
