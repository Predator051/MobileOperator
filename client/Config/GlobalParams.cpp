#include "GlobalParams.h"

std::string GlobalsParams::listenChannelAddress;
int GlobalsParams::listenChannelPort;

int GlobalsParams::getListenChannelPort()
{
    return listenChannelPort;
}

void GlobalsParams::setListenChannelPort(int value)
{
    listenChannelPort = value;
}

std::string GlobalsParams::getListenChannelAddress()
{
    return listenChannelAddress;
}

void GlobalsParams::setListenChannelAddress(const std::string &value)
{
    listenChannelAddress = value;
}
