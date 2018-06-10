#include "AdminView.h"
#include "ui_adminview.h"
#include <QCloseEvent>
#include "Helper.h"
#include <thread>
#include <chrono>

AdminView::AdminView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminView)
{
    qRegisterMetaType<std::string>("std::string");

    ui->setupUi(this);
    connect(this, SIGNAL(onReadData(std::string)), this, SLOT(readData(std::string)));
}

AdminView::~AdminView()
{
    delete ui;
}

std::shared_ptr<MessageManager> AdminView::message_manager() const
{
    return message_manager_;
}

void AdminView::setMessage_manager(const std::shared_ptr<MessageManager> &message_manager)
{
    message_manager_ = message_manager;
    if(message_manager)
    {
        message_manager_->setOnReadCB(std::bind(&AdminView::onRead, this, std::placeholders::_1));
        message_manager_->allUsers();
    }
}

void AdminView::onRead(const network::ResponseContext &response)
{
    emit onReadData(response.SerializeAsString());
}

void AdminView::displayListUsers(network::AllUsersResponse &res)
{
    if(res.phone_id_size() > 0)
        for(int i = 0; i<res.phone_id_size(); i++)
        {
            network::AllUsersResponse_PhoneId pi = res.phone_id(i);
            UserListItem* uli = new UserListItem(QString::fromStdString(pi.phone()));
            uli->phone = pi.phone();
            uli->user_id = pi.user_id();

            ui->lwUsers->addItem(uli);
        }
}

void AdminView::userPackageInfo(const network::UserPackageInfoResponse &res)
{
    ui->twPackageInfo->clear();
    ui->twPackageInfo->setRowCount(6);
    ui->twPackageInfo->setColumnCount(2);

    QTableWidget* tw = ui->twPackageInfo;

    tw->setItem(0, 0, new QTableWidgetItem("Phone"));
    tw->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(res.phone())));

    tw->setItem(1, 0, new QTableWidgetItem("Score"));
    tw->setItem(1, 1, new QTableWidgetItem(QString::number(res.score())));

    tw->setItem(2, 0, new QTableWidgetItem("Count of mb"));
    tw->setItem(2, 1, new QTableWidgetItem(QString::number(res.count_mb())));

    tw->setItem(3, 0, new QTableWidgetItem("Count of sms"));
    tw->setItem(3, 1, new QTableWidgetItem(QString::number(res.count_sms())));

    tw->setItem(4, 0, new QTableWidgetItem("Count sec into net"));
    tw->setItem(4, 1, new QTableWidgetItem(QString::number(res.count_sec_into_net())));

    tw->setItem(5, 0, new QTableWidgetItem("Count sec out net"));
    tw->setItem(5, 1, new QTableWidgetItem(QString::number(res.count_sec_out_net())));

    tw->resizeColumnsToContents();
    QTextBrowser* tb = ui->tbRateInfo;
    QString aboutRate;
    aboutRate += QString::fromStdString(res.user_rate().rate_info().about()) + ":";
    tb->setText(QString::fromStdString(Helper::beautyRateInfo(res.user_rate().rate_info())));

    QString aboutService;
    const network::UserServiceInfo& usi = res.user_service_info();
    LOG_INFO("Services: " << usi.user_services_size());
    for(int i = 0; i<usi.user_services_size(); i++)
    {
        const network::UserServiceInfo_UserService& us = usi.user_services(i);
        aboutService += QString::number(i) + ") " + QString::fromStdString(us.service().name()) + ":\n";
        aboutService += "  " + QString::fromStdString(us.service().about()) + "\n";

    }
    ui->tbUserService->setText(aboutService);
}

void AdminView::userStoryInfo(const network::UserPaidStoryResponse &res)
{
    int tableSize = res.rates_size() + res.sms_size() + res.services_size() + res.calls_size();
    ui->twHistory->clear();
    ui->twHistory->setRowCount(tableSize);
    ui->twHistory->setColumnCount(3);
    ui->twHistory->setHorizontalHeaderItem(0, new QTableWidgetItem("Date"));
    ui->twHistory->setHorizontalHeaderItem(1, new QTableWidgetItem("Content"));
    ui->twHistory->setHorizontalHeaderItem(2, new QTableWidgetItem("Paid"));

    LOG_INFO("History table size: " << tableSize);
    int j = 0;
    if(res.rates_size() > 0)
    {
        for(int i = 0; i<res.rates_size(); i++)
        {
            QString rate = QString::fromStdWString( Helper::wtime(res.rates(i).date()));
            ui->twHistory->setItem(j, 0, new QTableWidgetItem(rate));
            rate = " payment of a tariff " + QString::fromStdString(res.rates(i).rate().about());
            ui->twHistory->setItem(j, 1, new QTableWidgetItem(rate));
            ui->twHistory->setItem(j, 2, new QTableWidgetItem(QString::number(res.rates(i).rate().cost())));
            j++;
        }
    }

    if(res.services_size() > 0)
    {
        for(int i = 0; i<res.services_size(); i++)
        {
            QString rate = QString::fromStdWString( Helper::wtime(res.services(i).date()));
            ui->twHistory->setItem(j, 0, new QTableWidgetItem(rate));
            rate=" payment of a service " + QString::fromStdString(res.services(i).service().name());
            ui->twHistory->setItem(j, 1, new QTableWidgetItem(rate));
            ui->twHistory->setItem(j, 2, new QTableWidgetItem(QString::number(res.services(i).service().cost())));
            j++;
        }
    }

    if(res.sms_size() > 0)
    {
        for(int i = 0; i<res.sms_size(); i++)
        {
            QString rate = QString::fromStdWString( Helper::wtime(res.sms(i).date()));
            ui->twHistory->setItem(j, 0, new QTableWidgetItem(rate));
            rate=" payment of a sms ";
            ui->twHistory->setItem(j, 1, new QTableWidgetItem(rate));
            ui->twHistory->setItem(j, 2, new QTableWidgetItem(QString::number(res.sms(i).paid())));
            j++;
        }
    }

    if(res.calls_size() > 0)
    {
        for(int i = 0; i<res.calls_size(); i++)
        {
            QString rate = QString::fromStdWString( Helper::wtime(res.calls(i).date()));
            ui->twHistory->setItem(j, 0, new QTableWidgetItem(rate));
            rate=" payment of a calls to " + QString::fromStdString(res.calls(i).number_to());
            ui->twHistory->setItem(j, 1, new QTableWidgetItem(rate));
            ui->twHistory->setItem(j, 2, new QTableWidgetItem(QString::number(res.calls(i).paid())));
            j++;
        }
    }
    ui->twHistory->resizeColumnsToContents();
    ui->twHistory->resizeRowsToContents();
}

void AdminView::closeEvent(QCloseEvent *event)
{
    emit onClose(ExitAction::EXIT);
    event->accept();
}

void AdminView::readData(std::string str)
{
    network::ResponseContext response;
    response.ParseFromString(str);
    LOG_INFO("ADMIN: Response from server: " << response.error_code());

    switch (response.message_type_()) {
    case network::MO_ALL_USERS:
    {
        network::AllUsersResponse res = response.all_users_response();
        displayListUsers(res);
        break;
    }
    case network::MO_USER_PACKAGE_INFO:
    {
        network::UserPackageInfoResponse res = response.package_response();
        userPackageInfo(res);
        break;
    }
    case network::MO_USER_STORY:
    {
        network::UserPaidStoryResponse res = response.story_response();
        userStoryInfo(res);
        break;
    }
    }
}

void AdminView::on_lwUsers_itemClicked(QListWidgetItem *item)
{
    UserListItem* uli = static_cast<UserListItem*>(item);
    message_manager_->userPackage(uli->user_id);
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    message_manager_->userStory(uli->user_id);

}

void AdminView::on_pushButton_clicked()
{
    QTableWidget* leha = ui->twPackageInfo;
    network::AdminUpdateUserPackage auup;
    auup.set_count_mb(leha->item(2,1)->text().toInt());
    auup.set_count_sec_into_net(leha->item(4,1)->text().toInt());
    auup.set_count_sec_out_net(leha->item(5,1)->text().toInt());
    auup.set_count_sms(leha->item(3,1)->text().toInt());
    auup.set_score(leha->item(1,1)->text().toInt());

    uint64_t user_id = static_cast<UserListItem*>(ui->lwUsers->currentItem())->user_id;

    auup.set_user_id(user_id);
    message_manager_->adminUpdateUserPackage(auup);
}

void AdminView::on_pushButton_2_clicked()
{
    rsChangeView = std::make_shared<AdminRateService>();
    rsChangeView->setMessage_manager(message_manager_);
    rsChangeView->user_id = static_cast<UserListItem*>(ui->lwUsers->currentItem())->user_id;
    //connect(rsChangeView.get(), SIGNAL(onClose()), this, SLOT(onChangeViewClose()));
    rsChangeView->show();

    this->setEnabled(false);
}
