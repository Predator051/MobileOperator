#ifndef ADMINVIEW_H
#define ADMINVIEW_H

#include <QMainWindow>
#include "MessageManager.h"
#include "define.h"
#include <QListWidgetItem>
#include "AdminRateService.h"

namespace Ui {
class AdminView;
}

class UserListItem: public QListWidgetItem
{
public:
    UserListItem();
    UserListItem(const QString& str)
        : QListWidgetItem(str) {}

    std::string phone;
    uint64_t user_id;
};

class AdminView : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminView(QWidget *parent = 0);
    ~AdminView();

    std::shared_ptr<MessageManager> message_manager() const;
    void setMessage_manager(const std::shared_ptr<MessageManager> &message_manager);
    void onRead(const network::ResponseContext &response);

signals:
    void onClose(ExitAction);
    void onReadData(std::string);

private:
    Ui::AdminView *ui;
    std::shared_ptr<MessageManager> message_manager_;
    void displayListUsers(network::AllUsersResponse& res);
    void userPackageInfo(const network::UserPackageInfoResponse& res);
    void userStoryInfo(const network::UserPaidStoryResponse& res);

    std::shared_ptr<AdminRateService> rsChangeView;
    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void readData(std::string);
    void on_lwUsers_itemClicked(QListWidgetItem *item);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // ADMINVIEW_H
