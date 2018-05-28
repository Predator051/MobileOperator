#include "Helper.h"
#include <time.h>

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

uint64_t Helper::strDateToInt(const std::string &date)
{
    struct tm tm;
    strptime(date.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    return mktime(&tm);
}

//bool Helper::parceFromPostgres(const pqxx::tuple &data, NewUser &user)
//{
//    user.id = data["id"].as<int>();
//    user.name = data["name"].as<std::string>();
//    user.nick = data["nick"].as<std::string>();

//    LOG_INFO("Parced user: " << user);

//    return true;
//}

