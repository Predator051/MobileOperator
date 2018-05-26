#ifndef AUTHPOSTGRESMANAGER_H
#define AUTHPOSTGRESMANAGER_H

#include "DBHelper.h"
#include "define.h"
#include "Types/UserInfo.h"

class AuthPostgresManager
{
public:
    static ResponseCode isUserExist(const std::string& login, bool& exist);
    static ResponseCode createUser(const std::string& login, const std::string pass, const std::string &salt);
    static ResponseCode getUserByLogin(const std::string& login, UserInfo& userInfo);
    static ResponseCode getUserByUserId(uint64_t user_id, UserInfo& userInfo);
};

#endif // AUTHPOSTGRESMANAGER_H
