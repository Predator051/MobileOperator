#include "ConfigManager.h"
#include <string>
#include "GlobalParams.h"
#include "define.h"

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
}
