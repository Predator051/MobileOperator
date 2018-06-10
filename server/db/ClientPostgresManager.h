#ifndef CLIENTPOSTGRESMANAGER_H
#define CLIENTPOSTGRESMANAGER_H

#include "DBHelper.h"
#include "define.h"
#include "Types/UserInfo.h"

using PhoneUserId = std::pair<std::string, uint64_t>;

class ClientPostgresManager
{
public:
    static ResponseCode getUserInfo(uint64_t id, UserInfo& uInfo, PostgresRole role);
    static ResponseCode createUser(const UserInfo& uInfo, PostgresRole role);
    static ResponseCode updateUser(UserInfo& info, PostgresRole role);
    static ResponseCode getAllUsers(std::vector<PhoneUserId>& users, PostgresRole role);
};

#endif // CLIENTPOSTGRESMANAGER_H
