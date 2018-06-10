#ifndef RATESERVICECHANGEVIEW_H
#define RATESERVICECHANGEVIEW_H

#include <QWidget>
#include <MessageManager.h>
#include "View/ListItem.h"

namespace Ui {
class RateServiceChangeView;
}

class RateServiceChangeView : public QWidget
{
    Q_OBJECT

    std::shared_ptr<MessageManager> message_manager_;

signals:
    void onClose();
    void onReadData(std::string);

public:
    explicit RateServiceChangeView(QWidget *parent = 0);
    ~RateServiceChangeView();
    void onRead(const network::ResponseContext &response);


    std::shared_ptr<MessageManager> message_manager() const;
    void setMessage_manager(const std::shared_ptr<MessageManager> &message_manager);

private:
    Ui::RateServiceChangeView *ui;
    void displayAllRates(network::AllRatesResponse& res);
    void displayAllServices(network::AllServicesResponse& res);
    void ratePayment(network::RatePaymentResponse& res);
    void servicePayment(network::ServicePaymentResponse& res);
private slots:
    void readData(std::string);

    // QWidget interface
    void on_lwRates_itemClicked(QListWidgetItem *item);

    void on_lwServices_itemClicked(QListWidgetItem *item);

    void on_btnChangeRate_clicked();

    void on_btnChangeService_clicked();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // RATESERVICECHANGEVIEW_H
