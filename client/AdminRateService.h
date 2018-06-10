#ifndef ADMINRATESERVICE_H
#define ADMINRATESERVICE_H

#include <QWidget>
#include "MessageManager.h"
#include "define.h"
#include <QListWidgetItem>

namespace Ui {
class AdminRateService;
}

class AdminRateService : public QWidget
{
    Q_OBJECT

public:
    explicit AdminRateService(QWidget *parent = nullptr);
    ~AdminRateService();

    std::shared_ptr<MessageManager> message_manager() const;
    void setMessage_manager(const std::shared_ptr<MessageManager> &message_manager);
    void onRead(const network::ResponseContext &response);

    void ratePayment(network::AdminUserRateChangeResponse &res);
    void servicePayment(network::AdminUserServiceChangeResponse &res);

    uint64_t user_id;
signals:
    void onClose(ExitAction);
    void onReadData(std::string);
private:
    Ui::AdminRateService *ui;
    std::shared_ptr<MessageManager> message_manager_;
    void displayAllRates(network::AllRatesResponse& res);
    void displayAllServices(network::AllServicesResponse& res);

private slots:
    void readData(std::string);

    // QWidget interface
    void on_btnApplyRate_clicked();

    void on_lwRate_itemClicked(QListWidgetItem *item);

    void on_lwService_itemClicked(QListWidgetItem *item);

    void on_btnAddService_clicked();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // ADMINRATESERVICE_H
