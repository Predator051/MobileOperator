#include "AdminRateService.h"
#include "ui_AdminRateService.h"
#include <QCloseEvent>
#include "RateServiceChangeView.h"
#include <thread>
#include "Helper.h"

AdminRateService::AdminRateService(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminRateService)
{

    qRegisterMetaType<std::string>("std::string");
    ui->setupUi(this);

    connect(this, SIGNAL(onReadData(std::string)), this, SLOT(readData(std::string)));
}

AdminRateService::~AdminRateService()
{
    delete ui;
}

void AdminRateService::displayAllRates(network::AllRatesResponse &res)
{
    if(res.allrates_size() > 0)
    {
        for(int i = 0; i<res.allrates_size(); i++)
        {
            ListItemRate* rate = new ListItemRate(QString::fromStdString(res.allrates(i).about()));
            rate->rateInfo = res.allrates(i);
            ui->lwRate->addItem(rate);
        }
    }
}

void AdminRateService::displayAllServices(network::AllServicesResponse &res)
{
    if(res.allservices_size() > 0)
    {
        for(int i = 0; i<res.allservices_size(); i++)
        {
            ListItemService* rate = new ListItemService(QString::fromStdString(res.allservices(i).about()));
            rate->serviceInfo = res.allservices(i);
            ui->lwService->addItem(rate);
        }
    }
}

void AdminRateService::ratePayment(network::AdminUserRateChangeResponse &res)
{
    ui->lbResponse->setText("Rate: " + QString::fromStdString(res.status_message()));
}

void AdminRateService::servicePayment(network::AdminUserServiceChangeResponse &res)
{
    ui->lbResponse->setText("Service: " + QString::fromStdString(res.status_message()));
}


void AdminRateService::readData(std::string str)
{
    network::ResponseContext response;
    response.ParseFromString(str);
    LOG_INFO("CLIENT: Response from server: " << response.error_code());

    switch (response.message_type_()) {
    case network::MO_ALL_RATES:
    {
        network::AllRatesResponse res = response.all_rates();
        displayAllRates(res);
        break;
    }
    case network::MO_ALL_SERVICES:
    {
        network::AllServicesResponse res = response.all_services();
        displayAllServices(res);
        break;
    }
    case network::MO_ADMIN_USER_SERVICE_CHANGE:
    {
        network::AdminUserServiceChangeResponse res = response.admin_user_service();
        servicePayment(res);
        break;
    }
    case network::MO_ADMIN_USER_RATE_CHANGE:
    {
        network::AdminUserRateChangeResponse res = response.admin_user_rate_change();
        ratePayment(res);
        break;
    }

    }
}

std::shared_ptr<MessageManager> AdminRateService::message_manager() const
{
    return message_manager_;
}


void AdminRateService::setMessage_manager(const std::shared_ptr<MessageManager> &message_manager)
{
    message_manager_ = message_manager;
    if(message_manager)
    {
        message_manager_->setOnReadCB(std::bind(&AdminRateService::onRead, this, std::placeholders::_1));
        message_manager_->allRates();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        message_manager_->allService();
    }
    ui->btnAddService->setEnabled(false);
    ui->btnApplyRate->setEnabled(false);
}

void AdminRateService::onRead(const network::ResponseContext &response)
{
    emit onReadData(response.SerializeAsString());
}

void AdminRateService::closeEvent(QCloseEvent *event)
{
    emit onClose(ExitAction::EXIT);
    event->accept();
}

void AdminRateService::on_btnApplyRate_clicked()
{
    ListItemRate* lir = static_cast<ListItemRate*>(ui->lwRate->currentItem());
    message_manager_->adminRateChange(lir->rateInfo.id(), user_id);
}

void AdminRateService::on_lwRate_itemClicked(QListWidgetItem *item)
{
    ListItemRate* lir = static_cast<ListItemRate*>(item);
    ui->tbRate->setText(QString::fromStdString(Helper::beautyRateInfo(lir->rateInfo)));
    ui->btnApplyRate->setEnabled(true);
}

void AdminRateService::on_lwService_itemClicked(QListWidgetItem *item)
{
    ListItemService* lir = static_cast<ListItemService*>(item);
    ui->tbService->setText(QString::fromStdString(Helper::beautyServiceInfo(lir->serviceInfo)));
    ui->btnAddService->setEnabled(true);
}

void AdminRateService::on_btnAddService_clicked()
{
    ListItemService* lir = static_cast<ListItemService*>(ui->lwService->currentItem());
    message_manager_->adminServiceChange(lir->serviceInfo.id(), user_id);
}
