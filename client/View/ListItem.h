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
};

class ListItemService: public QListWidgetItem
{
public:
    ListItemService();
    ListItemService(QString text);
    network::ServiceInfo serviceInfo;
};



#endif // LISTITEM_H
