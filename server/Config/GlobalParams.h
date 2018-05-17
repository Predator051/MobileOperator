#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <string>

struct postgres_role
{
    std::string username;
    std::string password;
    std::string dbname;
};

class GlobalsParams
{
    static std::string listenChannelAddress;
    static int listenChannelPort;
    static postgres_role clientRole;
    static postgres_role adminRole;
    static postgres_role authRole;
    static std::string postgres_host;
    static std::string postgres_port;
    static std::string postgres_default_solt;

public:
    static std::string getListenChannelAddress();
    static void setListenChannelAddress(const std::string &value);
    static int getListenChannelPort();
    static void setListenChannelPort(int value);

    static postgres_role getClientRole();
    static void setClientRole(const postgres_role &value);
    static postgres_role getAdminRole();
    static void setAdminRole(const postgres_role &value);
    static postgres_role getAuthRole();
    static void setAuthRole(const postgres_role &value);
    static std::string getPostgres_port();
    static void setPostgres_port(const std::string &value);
    static std::string getPostgres_host();
    static void setPostgres_host(const std::string &value);
    static std::string getPostgres_default_solt();
    static void setPostgres_default_solt(const std::string &value);
};
#endif // GLOBALPARAMS_H
