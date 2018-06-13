#ifndef AUTHLOGIC_H
#define AUTHLOGIC_H

#include "define.h"
#include "Protobuf/Message.pb.h"
#include "Types/UserInfo.h"

class AuthLogic
{
public:
    static ResponseCode createUser(const network::RegisterMessage& authData, network::RegisterMessageResponse *response, PostgresRole role);
    static ResponseCode authUser(const network::AuthMessage& authData, network::AuthMessageResponse *response, network::SessionInfo *sessionInfo);
    static ResponseCode logout(const network::RequestContext &request);
    static ResponseCode getPackageState(uint64_t user_id, User& user, PostgresRole role);
    static ResponseCode getUserStory(uint64_t user_id, UserStory& story, PostgresRole role);
    static ResponseCode getAllRates(uint64_t user_id, std::vector<network::RateInfo> &rates, PostgresRole role);
    static ResponseCode getAllServices(uint64_t user_id, std::vector<network::ServiceInfo>& services, PostgresRole role);
    static ResponseCode ratePayment(uint64_t user_id, network::RatePaymentMessage& rpm, network::RatePaymentResponse& response, PostgresRole role);
    static ResponseCode servicePayment(uint64_t user_id, network::ServicePaymentMessage& rpm, network::ServicePaymentResponse& response, PostgresRole role);
    static ResponseCode getAllUsers(std::vector<std::pair<std::string, uint64_t> > &result, PostgresRole role);
    static ResponseCode adminUpdateUserPackage(const network::AdminUpdateUserPackage& auup, network::AdminUpdateUserPackageResponse& res);
    static ResponseCode adminUserRateChange(const network::AdminUserRateChange& auup, network::AdminUserRateChangeResponse& res);
    static ResponseCode adminUserServiceChange(const network::AdminUserServiceChange& auup, network::AdminUserServiceChangeResponse& res);
    static ResponseCode rateStatistics(std::vector<network::RateStatisticsResponse_MonthCount> &res, PostgresRole role);
    static ResponseCode servicetatistics(std::vector<network::ServiceStatisticsResponse_MonthCount> &res, PostgresRole role);


};

#endif // AUTHLOGIC_H
