#ifndef USERINFO_H
#define USERINFO_H

#include <string>
#include <pqxx/pqxx>
#include <Types/RateInfo.h>
#include <Protobuf/Message.pb.h>

struct PackageInfo
{
    uint32_t count_sec_into_net;
    uint32_t count_sec_out_net;
    uint32_t count_sms;
    uint32_t count_mb;
    float score;

    void parse_from_pg(const pqxx::tuple& value);
};

class UserInfo
{
public:
    UserInfo();
    uint64_t user_id;
    std::string user_login;
    std::string password;
    uint64_t creation_time;
    int32_t role;
    std::string salt;
    float score;

    void parse_from_pg(const pqxx::tuple& value);
    void parse_from_mo_db(const pqxx::tuple& value);
};

using ServiceInfoDate = std::pair<network::ServiceInfo, uint64_t>;

struct User
{
    PackageInfo package;
    UserInfo user_info;
    RateInfo rate_info;
    std::vector<ServiceInfoDate> services;
};

struct UserStory
{
    std::vector<network::UserConnectedRate> rates;
    std::vector<network::UserConnectedService> services;
    std::vector<network::SmsPayment> sms;
    std::vector<network::OutboundCallPayment> calls;
};

#endif // USERINFO_H
