#ifndef LISTITEM_H
#define LISTITEM_H

#include <QListWidgetItem>
#include <Protobuf/Message.pb.h>

class ListItemRate: public QListWidgetItem
{
public:
    ListItemRate();
    ListItemRate(QString text);
    network::RateInfo rateInfo;
    std::vector<network::RateStatisticsResponse_MonthCount> statistics;
};

class ListItemService: public QListWidgetItem
{
public:
    ListItemService();
    ListItemService(QString text);
    network::ServiceInfo serviceInfo;
    std::vector<network::ServiceStatisticsResponse_MonthCount> statistics;
};



#endif // LISTITEM_H
