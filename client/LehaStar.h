#ifndef LEHASTAR_H
#define LEHASTAR_H

#include <QWidget>
#include "MessageManager.h"
#include "ClientView.h"
#include "AdminView.h"

namespace Ui {
class LehaStar;
}

class LehaStar : public QWidget
{
    Q_OBJECT

public:
    explicit LehaStar(std::shared_ptr<MessageManager> message_manager, QWidget *parent = 0);
    ~LehaStar();
    void onError(ClientError error);
    void onRead(const network::ResponseContext &response);

private slots:
    void on_testBtn_clicked();
    void updateTime();
    void on_testBtn_2_clicked();
    void logout();

private:
    Ui::LehaStar *ui;
    std::shared_ptr<MessageManager> message_manager_;
    std::shared_ptr<ClientView> clientView_;
    std::shared_ptr<AdminView> adminView_;

    void cannotConnectError();
    void disconnectError();

    void userRegister(const network::RegisterMessageResponse& response);
    void userAuth(const network::AuthMessageResponse& authMessage
                  , const network::SessionInfo& sessionInfo);
    QTimer* timer_;
};

#endif // LEHASTAR_H
