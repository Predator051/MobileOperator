#include "ProtoHelper.h"
#include "Helper.h"

void ProtoHelper::parse_from_pg(const pqxx::tuple &value, network::ServiceInfo &info)
{
    info.set_about(value["about"].as<std::string>());
    info.set_cost(value["cost"].as<float>());
    info.set_id(value["id"].as<uint32_t>());
    info.set_count_mb(value["count_mb"].as<uint32_t>());
    info.set_count_sec_into_net(value["count_sec_into_net"].as<uint32_t>());
    info.set_count_sec_out_net(value["count_sec_out_net"].as<uint32_t>());
    info.set_live_time(value["live_time"].as<uint32_t>());
    info.set_name(value["name"].as<std::string>());
}

void ProtoHelper::parse_from_pg(const pqxx::tuple &value, network::SmsPayment &sms)
{
    sms.set_date(Helper::strDateToInt(value["date"].as<std::string>()));
    sms.set_paid(value["paid"].as<float>());
    sms.set_user_id(value["user_id"].as<int64_t>());
}

void ProtoHelper::parse_from_pg(const pqxx::tuple &value, network::OutboundCallPayment &out)
{
    out.set_count_sec(value["count_sec"].as<uint64_t>());
    out.set_date(Helper::strDateToInt(value["date"].as<std::string>()));
    out.set_number_to(value["number_to"].as<std::string>());
    out.set_paid(value["paid"].as<float>());
    out.set_user_id(value["user_id_from"].as<uint64_t>());
}
