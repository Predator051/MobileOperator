#include "UserInfo.h"

UserInfo::UserInfo()
{

}

void UserInfo::parse_from_pg(const pqxx::tuple &value)
{
    user_id = value["user_id"].as<int64_t>();
    user_login = value["user_login"].as<std::string>();
    password = value["password"].as<std::string>();
    role = value["role"].as<int32_t>();
    salt = value["salt"].as<std::string>();
    //TODO creation_time
}

void UserInfo::parse_from_mo_db(const pqxx::tuple &value)
{
    user_id = value["id"].as<int64_t>();
    score = value["id"].as<float>();
    user_login = value["phone"].as<std::string>();
}

void PackageInfo::parse_from_pg(const pqxx::tuple &value)
{
    count_mb = value["count_mb"].as<uint32_t>();
    count_sec_into_net = value["count_sec_into_net"].as<uint32_t>();
    count_sec_out_net = value["count_sec_out_net"].as<uint32_t>();
    count_sms = value["count_sms"].as<uint32_t>();
}
