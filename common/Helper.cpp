#include "Helper.h"
#include <ctime>
#include <sstream>
#include <iomanip>

ByteBufferPtr Helper::makeBuffer(ByteBufferPtr buffPtr)
{
    uint16_t msgSize = buffPtr->size();

    uint16_t lowByte = msgSize & 0x00FF;
    uint16_t highByte = msgSize >> 8;

    buffPtr->emplace(buffPtr->begin(), static_cast<char>(lowByte));
    buffPtr->emplace(buffPtr->begin(), static_cast<char>(highByte));

    return buffPtr;
}

uint16_t Helper::mergeTwoByte(uint8_t high, uint8_t low)
{
    uint16_t number = (high << 8) | low;
    return number;
}

ByteBuffer Helper::stringToBuffer(const std::string &str)
{
    return ByteBuffer(str.begin(), str.end());
}

std::string Helper::bufferToString(ByteBufferPtr buffPtr, uint posFrom, uint posTo)
{
    return std::string(buffPtr->begin()+posFrom, buffPtr->end() - posTo);
}

std::wstring Helper::wtime(const time_t &t) {
    std::tm tm = *std::localtime(&t);
    std::wstringstream wss;
    wss << std::put_time(&tm, L"%F %T");
    return wss.str();
}

std::string Helper::beautyRateInfo(const network::RateInfo &rateInfo)
{
    std::stringstream str;
    str << rateInfo.about() << ":" << std::endl;
    str << "Count megabyte: " << rateInfo.count_mb() << std::endl;
    str << "Count seconds into our net: " << rateInfo.count_sec_into_net() << std::endl;
    str << "Count seconds out our net: " << rateInfo.count_sec_out_net() << std::endl;
    str << "Count sms: " << rateInfo.count_sms() << std::endl;
    str << "If you have insufficient funds: " << std::endl;
    str << "Price for one sms: " << rateInfo.price_sms() << std::endl;
    str << "Price into an our net for one second: " << rateInfo.price_into_net() << std::endl;
    str << "Price out an our net for one second: " << rateInfo.price_out_net() << std::endl;
    str << "COST :" << rateInfo.cost() << std::endl;
    return str.str();
}

std::string Helper::beautyServiceInfo(const network::ServiceInfo &serviceInfo)
{
    std::stringstream str;
    str << serviceInfo.about() << ":" << std::endl;
    str << "Count megabyte: " << serviceInfo.count_mb() << std::endl;
    str << "Count seconds into our net: " << serviceInfo.count_sec_into_net() << std::endl;
    str << "Count seconds out our net: " << serviceInfo.count_sec_out_net() << std::endl;
    str << "Count sms: " << serviceInfo.count_sms() << std::endl;
    str << "COST :" << serviceInfo.cost() << std::endl;
    return str.str();
}

//bool Helper::parceFromPostgres(const pqxx::tuple &data, NewUser &user)
//{
//    user.id = data["id"].as<int>();
//    user.name = data["name"].as<std::string>();
//    user.nick = data["nick"].as<std::string>();

//    LOG_INFO("Parced user: " << user);

//    return true;
//}

