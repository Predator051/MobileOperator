#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <string>

class GlobalsParams
{
    static std::string listenChannelAddress;
    static int listenChannelPort;
    static bool isConnected;

public:
    static std::string getListenChannelAddress();
    static void setListenChannelAddress(const std::string &value);
    static int getListenChannelPort();
    static void setListenChannelPort(int value);
    static bool getIsConnected();
    static void setIsConnected(bool value);
};
#endif // GLOBALPARAMS_H
