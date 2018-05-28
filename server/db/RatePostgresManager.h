#ifndef RATEPOSTGRESMANAGER_H
#define RATEPOSTGRESMANAGER_H

#include "DBHelper.h"
#include "define.h"
#include "Types/RateInfo.h"

class RatePostgresManager
{
public:
    static ResponseCode getRateInfo(uint64_t id, RateInfo& rate);
    static ResponseCode getUserRate(uint64_t user_id, uint64_t& rate_id, uint64_t &connected_date);
};

#endif // RATEPOSTGRESMANAGER_H
