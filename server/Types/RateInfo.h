#ifndef RATEINFO_H
#define RATEINFO_H

#include <string>
#include <pqxx/pqxx>
#include <Protobuf/Message.pb.h>

class RateInfo
{
public:
    int32_t count_sec_into_net;
    int32_t count_sec_out_net;
    int32_t count_sms;
    int32_t count_mb;
    int32_t price_into_net;
    int32_t price_out_net;
    int32_t price_sms;
    int32_t period;
    std::string about;
    uint32_t id;
    float cost;
    uint64_t connected_date;

    void parse_from_pg(const pqxx::tuple& value);
    void serialize_to_pb(network::RateInfo* rateInfo);
};

#endif // RATEINFO_H
