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
