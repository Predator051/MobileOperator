#ifndef AUTHLOGIC_H
#define AUTHLOGIC_H

#include "define.h"
#include "Protobuf/Message.pb.h"
#include "Types/UserInfo.h"

class AuthLogic
{
public:
    static ResponseCode createUser(const network::RegisterMessage& authData, network::RegisterMessageResponse *response);
    static ResponseCode authUser(const network::AuthMessage& authData, network::AuthMessageResponse *response, network::SessionInfo *sessionInfo);
    static ResponseCode logout(const network::RequestContext &request);
    static ResponseCode getPackageState(uint64_t user_id, User& user);
};

#endif // AUTHLOGIC_H
