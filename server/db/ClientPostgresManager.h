#ifndef CLIENTPOSTGRESMANAGER_H
#define CLIENTPOSTGRESMANAGER_H

#include "DBHelper.h"
#include "define.h"
#include "Types/UserInfo.h"

class ClientPostgresManager
{
public:
    static ResponseCode getUserInfo(uint64_t id, UserInfo& uInfo);
    static ResponseCode createUser(const UserInfo& uInfo);
};

#endif // CLIENTPOSTGRESMANAGER_H
