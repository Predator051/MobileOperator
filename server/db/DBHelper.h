#ifndef DBHELPER_H
#define DBHELPER_H

#include <pqxx/pqxx>
#include <memory>
#include <atomic>
#include <mutex>

using db_connection_ptr = std::shared_ptr<pqxx::connection>;

class DBHelper
{
public:
    static db_connection_ptr getClientConnection();
    static db_connection_ptr getAdminConnection();
    static db_connection_ptr getAuthConnection();

    static DBHelper& getDBHelper();

    void prepareStatetment(const std::string& str);
    bool isPrepared(const std::string& str);
private:
    static db_connection_ptr makeConnection(const std::string& host, const std::string& port
                                              , const std::string& user, const std::string& user_pass
                                              , const std::string& db);

    std::vector<std::string> prepared_statetment;
    std::mutex mutex;
};

#endif // DBHELPER_H
