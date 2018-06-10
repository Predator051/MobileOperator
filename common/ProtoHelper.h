#ifndef PROTOHELPER_H
#define PROTOHELPER_H

#include "Protobuf/Message.pb.h"
#include <pqxx/pqxx>

class ProtoHelper
{
public:
    static void parse_from_pg(const pqxx::tuple& value, network::ServiceInfo& info);
    static void parse_from_pg(const pqxx::tuple& value, network::SmsPayment& sms);
    static void parse_from_pg(const pqxx::tuple& value, network::OutboundCallPayment& out);

};

#endif // PROTOHELPER_H
