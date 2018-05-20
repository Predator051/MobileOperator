#ifndef USERINFO_H
#define USERINFO_H

#include <string>
#include <pqxx/pqxx>

class UserInfo
{
public:
    UserInfo();
    uint64_t user_id;
    std::string user_login;
    std::string password;
    uint64_t creation_time;
    int32_t role;
    std::string salt;

    void parse_from_pg(const pqxx::tuple& value);
};

#endif // USERINFO_H
