#include "ClientView.h"
#include "ui_clientview.h"
#include <QCloseEvent>
#include <Helper.h>
#include <mutex>
#include <thread>
#include <chrono>
#include <GlobalParams.h>

ClientView::ClientView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientView)
{
    qRegisterMetaType<std::string>("std::string");
    ui->setupUi(this);
    connect(this, SIGNAL(onReadData(std::string)), this, SLOT(readData(std::string)));
}

ClientView::~ClientView()
{
    delete ui;
}

std::shared_ptr<MessageManager> ClientView::message_manager() const
{
    return message_manager_;
}

void ClientView::setMessage_manager(std::shared_ptr<MessageManager> message_manager)
{
    message_manager_ = message_manager;
    if(message_manager)
    {
        message_manager_->setOnReadCB(std::bind(&ClientView::onRead, this, std::placeholders::_1));
        message_manager_->userPackage(GlobalsParams::getSessionInfo().userid());
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        message_manager_->userStory(GlobalsParams::getSessionInfo().userid());
    }
}

void ClientView::onRead(const network::ResponseContext &response)
{
    emit onReadData(response.SerializeAsString());
}

void ClientView::userPackageInfo(const network::UserPackageInfoResponse &res)
{
//    QTableWidgetItem mbText("Count of mb");
//    QTableWidgetItem intoNetText("Count of sec into net");
//    QTableWidgetItem outNetText("Count of sec out net");
//    QTableWidgetItem phoneText("Phone");
//    QTableWidgetItem smsText("Count of sms");
//    QTableWidgetItem scoreText("Score");

//    QTableWidgetItem mb(QString::number(res.count_mb()));
//    QTableWidgetItem intoNet(QString::number(res.count_sec_into_net()));
//    QTableWidgetItem outNet(QString::number(res.count_sec_out_net()));
//    QTableWidgetItem phone(QString::fromStdString(res.phone()));
//    QTableWidgetItem sms(QString::number(res.count_sms()));
//    QTableWidgetItem score(QString::number(res.score()));

    ui->twPackageInfo->clear();
    ui->twPackageInfo->setRowCount(6);

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
    ui->tbMyService->setText(aboutService);
}

void ClientView::userStoryInfo(const network::UserPaidStoryResponse &res)
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
            rate = "| payment of a tariff " + QString::fromStdString(res.rates(i).rate().about());
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
            rate="| payment of a service " + QString::fromStdString(res.services(i).service().name());
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
            rate="| payment of a sms ";
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
            rate="| payment of a calls to " + QString::fromStdString(res.calls(i).number_to());
            ui->twHistory->setItem(j, 1, new QTableWidgetItem(rate));
            ui->twHistory->setItem(j, 2, new QTableWidgetItem(QString::number(res.calls(i).paid())));
            j++;
        }
    }
    ui->twHistory->resizeColumnsToContents();
    ui->twHistory->resizeRowsToContents();
}

void ClientView::closeEvent(QCloseEvent *event)
{
    emit onClose(ExitAction::EXIT);
    event->accept();
}

void ClientView::on_actionEXIT_2_triggered()
{
    this->close();
}

void ClientView::readData(std::string str)
{
    //std::lock_guard<std::mutex> guard(mutex, std::adopt_lock);
    network::ResponseContext response;
    response.ParseFromString(str);
    LOG_INFO("CLIENT: Response from server: " << response.error_code());

    switch (response.message_type_()) {
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

void ClientView::on_actionRATES_SERVICE_triggered()
{
    rsChangeView = std::make_shared<RateServiceChangeView>();
    rsChangeView->setMessage_manager(message_manager_);
    connect(rsChangeView.get(), SIGNAL(onClose()), this, SLOT(onChangeViewClose()));
    rsChangeView->show();

    this->setEnabled(false);
}

void ClientView::onChangeViewClose()
{
    this->setEnabled(true);
    message_manager_->setOnReadCB(std::bind(&ClientView::onRead, this, std::placeholders::_1));
    message_manager_->userPackage(GlobalsParams::getSessionInfo().userid());

    std::this_thread::sleep_for(std::chrono::microseconds(10));
    message_manager_->userStory(GlobalsParams::getSessionInfo().userid());
}

void ClientView::on_actionEXIT_triggered()
{
    emit onClose(ExitAction::LOGOUT);
    this->hide();
}
