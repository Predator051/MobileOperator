#ifndef USERPACKAGEPOSTGRESMANAGER_H
#define USERPACKAGEPOSTGRESMANAGER_H

#include "DBHelper.h"
#include "define.h"
#include "Types/UserInfo.h"

class UserPackagePostgresManager
{
public:
    static ResponseCode getPackage(PackageInfo& packageInfo, uint32_t user_id);
};

#endif // USERPACKAGEPOSTGRESMANAGER_H
