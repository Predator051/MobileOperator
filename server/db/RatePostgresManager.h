#ifndef RATEPOSTGRESMANAGER_H
#define RATEPOSTGRESMANAGER_H

#include "DBHelper.h"
#include "define.h"
#include "Types/RateInfo.h"

using RateDate = std::pair<uint64_t, uint64_t>;

class RatePostgresManager
{
public:
    static ResponseCode getRateInfo(uint64_t id, RateInfo& rate, PostgresRole role);
    static ResponseCode getUserRate(uint64_t user_id, uint64_t& rate_id, uint64_t &connected_date, PostgresRole role);
    static ResponseCode getUserConnectedRate(uint64_t user_id, std::vector<RateDate> &rates, PostgresRole role);
    static ResponseCode getAllRates(std::vector<RateInfo>& rates, PostgresRole role);
    static ResponseCode createUserRate(uint64_t user_id, uint64_t rate_id, PostgresRole role);
};

#endif // RATEPOSTGRESMANAGER_H
