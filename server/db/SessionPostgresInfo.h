#ifndef SESSIONPOSTGRESINFO_H
#define SESSIONPOSTGRESINFO_H

#include "DBHelper.h"
#include "define.h"
#include "Types/UserInfo.h"

class SessionPostgresInfo
{
public:
    static ResponseCode createSession(const uint64_t& user_id, const std::string& session_id);
    static ResponseCode removeSession(const std::string& session_id);
    static ResponseCode isSessionExist(const uint64_t& user_id, bool &isExist);
    static ResponseCode getSession(const uint64_t& user_id, std::string& session_id);

};

#endif // SESSIONPOSTGRESINFO_H
