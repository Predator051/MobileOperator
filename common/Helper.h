#ifndef HELPER_H
#define HELPER_H

#include "define.h"
//#include "pqxx/pqxx"
#include "Protobuf/Message.pb.h"

class Helper
{
public:
    Helper();
    static ByteBufferPtr makeBuffer(ByteBufferPtr message);
    static uint16_t mergeTwoByte(uint8_t high, uint8_t low);
    static ByteBuffer stringToBuffer(const std::string& str);
    static std::string bufferToString(ByteBufferPtr buffPtr, uint posFrom, uint posTo = 0);
    //static bool parceFromPostgres(const pqxx::tuple& data, NewUser& user);
    static std::wstring wtime(const time_t &t);
    static std::string beautyRateInfo(const network::RateInfo& rateInfo);
    static std::string beautyServiceInfo(const network::ServiceInfo& serviceInfo);
};

#endif // HELPER_H
