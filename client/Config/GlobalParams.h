#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <string>
#include "Protobuf/Message.pb.h"

class GlobalsParams
{
    static std::string listenChannelAddress;
    static int listenChannelPort;
    static bool isConnected;

    static network::SessionInfo sessionInfo;
public:
    static std::string getListenChannelAddress();
    static void setListenChannelAddress(const std::string &value);
    static int getListenChannelPort();
    static void setListenChannelPort(int value);
    static bool getIsConnected();
    static void setIsConnected(bool value);
    static network::SessionInfo &getSessionInfo();
};
#endif // GLOBALPARAMS_H
