#include "RateInfo.h"

void RateInfo::parse_from_pg(const pqxx::tuple &value)
{
    count_mb = value["count_mb"].as<int32_t>();
    count_sec_into_net = value["count_sec_into_net"].as<int32_t>();
    count_sms = value["count_sms"].as<int32_t>();
    count_sec_out_net = value["count_sec_out_net"].as<int32_t>();
    price_into_net = value["into_net_price"].as<int32_t>();
    price_out_net = value["out_net_price"].as<int32_t>();
    price_sms = value["sms_price"].as<int32_t>();
    period = value["period"].as<int32_t>();
    about = value["about"].as<std::string>();
    id = value["id"].as<int32_t>();
    cost = value["cost"].as<float>();
}

void RateInfo::serialize_to_pb(network::RateInfo *rateInfo)
{
    rateInfo->set_about(about);
    rateInfo->set_count_mb(count_mb);
    rateInfo->set_count_sec_into_net(count_sec_into_net);
    rateInfo->set_count_sms(count_sms);
    rateInfo->set_count_sec_out_net(count_sec_out_net);
    rateInfo->set_price_into_net(price_into_net);
    rateInfo->set_price_out_net(price_out_net);
    rateInfo->set_price_sms(price_sms);
    rateInfo->set_period(period);
    rateInfo->set_about(about);
    rateInfo->set_cost(cost);
}
