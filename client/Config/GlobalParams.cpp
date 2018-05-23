#include "GlobalParams.h"

std::string GlobalsParams::listenChannelAddress;
int GlobalsParams::listenChannelPort;
bool GlobalsParams::isConnected;
network::SessionInfo GlobalsParams::sessionInfo;

int GlobalsParams::getListenChannelPort()
{
    return listenChannelPort;
}

void GlobalsParams::setListenChannelPort(int value)
{
    listenChannelPort = value;
}

bool GlobalsParams::getIsConnected()
{
    return isConnected;
}

void GlobalsParams::setIsConnected(bool value)
{
    isConnected = value;
}

network::SessionInfo& GlobalsParams::getSessionInfo()
{
    return sessionInfo;
}

std::string GlobalsParams::getListenChannelAddress()
{
    return listenChannelAddress;
}

void GlobalsParams::setListenChannelAddress(const std::string &value)
{
    listenChannelAddress = value;
}
