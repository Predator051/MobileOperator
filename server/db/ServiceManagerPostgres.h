#ifndef SERVICEMANAGERPOSTGRES_H
#define SERVICEMANAGERPOSTGRES_H

#include "DBHelper.h"
#include "define.h"
#include "Protobuf/Message.pb.h"
#include "ProtoHelper.h"

using ServiceDate = std::pair<int64_t, uint64_t>;

class ServiceManagerPostgres
{
public:
    static ResponseCode getServiceInfo(int64_t id, network::ServiceInfo& serviceInfo, PostgresRole role);
    static ResponseCode getUserServices(int64_t id, std::vector<ServiceDate>& services, PostgresRole role);
    static ResponseCode getUserConnectedService(int64_t id, std::vector<ServiceDate>& services, PostgresRole role);
    static ResponseCode getAllServices(std::vector<network::ServiceInfo>& services, PostgresRole role);
    static ResponseCode createUserService(int64_t user_id, int64_t service_id, PostgresRole role);
};

#endif // SERVICEMANAGERPOSTGRES_H
