#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QMainWindow>
#include "MessageManager.h"
#include <memory>

namespace Ui {
class ClientView;
}

class ClientView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientView(QWidget *parent = 0);
    ~ClientView();

    std::shared_ptr<MessageManager> message_manager() const;
    void setMessage_manager(std::shared_ptr<MessageManager> message_manager);
    void onRead(const network::ResponseContext &response);

signals:
    void onClose();
    void onReadData(std::string);

private:
    Ui::ClientView *ui;
    std::shared_ptr<MessageManager> message_manager_;
    void userPackageInfo(const network::UserPackageInfoResponse& res);
    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_actionEXIT_2_triggered();
    void readData(std::string);
};

#endif // CLIENTVIEW_H
