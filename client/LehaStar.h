#ifndef LEHASTAR_H
#define LEHASTAR_H

#include <QWidget>
#include "MessageManager.h"

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
private slots:
    void on_testBtn_clicked();
    void updateTime();
private:
    Ui::LehaStar *ui;
    std::shared_ptr<MessageManager> message_manager_;

    void cannotConnectError();
    void disconnectError();
    QTimer* timer_;
};

#endif // LEHASTAR_H
