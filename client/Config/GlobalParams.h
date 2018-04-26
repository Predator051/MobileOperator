#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <string>

class GlobalsParams
{
    static std::string listenChannelAddress;
    static int listenChannelPort;


public:
    static std::string getListenChannelAddress();
    static void setListenChannelAddress(const std::string &value);
    static int getListenChannelPort();
    static void setListenChannelPort(int value);
};
#endif // GLOBALPARAMS_H
