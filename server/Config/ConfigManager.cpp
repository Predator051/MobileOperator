#include "ConfigManager.h"
#include <string>
#include "GlobalParams.h"
#include "define.h"

void parse_db_params(const libconfig::Config& cfg);

ConfigManager::ConfigManager()
{
}

bool ConfigManager::readConfigFIle(const std::string &path)
{
    try
    {
        conf.readFile(path.c_str());
        return true;
    }
    catch(libconfig::ParseException e)
    {
        LOG_ERR("Failure read config file: " << e.what());
    }
    return false;
}

void ConfigManager::parseConfigs()
{
    if(GlobalsParams::getListenChannelAddress().empty())
        GlobalsParams::setListenChannelAddress(conf.lookup("application.network.listen_address"));

    if(GlobalsParams::getListenChannelPort() == 0)
        GlobalsParams::setListenChannelPort(conf.lookup("application.network.listen_port"));

    parse_db_params(conf);
}


void parse_db_params(const libconfig::Config& cfg)
{
    postgres_role admin;
    admin.dbname = cfg.lookup("application.postgres.admin.dbname").c_str();
    admin.password = cfg.lookup("application.postgres.admin.password").c_str();
    admin.username = cfg.lookup("application.postgres.admin.user").c_str();

    GlobalsParams::setAdminRole(admin);

    postgres_role client;
    client.dbname = cfg.lookup("application.postgres.client.dbname").c_str();
    client.password = cfg.lookup("application.postgres.client.password").c_str();
    client.username = cfg.lookup("application.postgres.client.user").c_str();

    GlobalsParams::setClientRole(client);

    postgres_role auth;
    auth.dbname = cfg.lookup("application.postgres.auth.dbname").c_str();
    auth.password = cfg.lookup("application.postgres.auth.password").c_str();
    auth.username = cfg.lookup("application.postgres.auth.user").c_str();

    GlobalsParams::setAuthRole(auth);

    GlobalsParams::setPostgres_host(cfg.lookup("application.postgres.host"));
    GlobalsParams::setPostgres_port(cfg.lookup("application.postgres.port"));
    GlobalsParams::setPostgres_default_solt(cfg.lookup("application.postgres.salt"));
}
