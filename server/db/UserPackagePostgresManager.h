#ifndef USERPACKAGEPOSTGRESMANAGER_H
#define USERPACKAGEPOSTGRESMANAGER_H

#include "DBHelper.h"
#include "define.h"
#include "Types/UserInfo.h"
#include "Protobuf/Message.pb.h"

class UserPackagePostgresManager
{
public:
    static ResponseCode getPackage(PackageInfo& packageInfo, uint32_t user_id, PostgresRole role);
    static ResponseCode getSmsPayment(uint64_t user_id, std::vector<network::SmsPayment>& sms, PostgresRole role);
    static ResponseCode getOutboundCalls(uint64_t user_id, std::vector<network::OutboundCallPayment>& out, PostgresRole role);
    static ResponseCode updatePackage(PackageInfo& packageInfo, uint32_t user_id, PostgresRole role);
    static ResponseCode updatePackageAdmin(PackageInfo& packageInfo, uint32_t user_id);
    static ResponseCode createEmptyPackage(uint32_t user_id, PostgresRole role);

};

#endif // USERPACKAGEPOSTGRESMANAGER_H
