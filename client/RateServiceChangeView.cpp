#include "RateServiceChangeView.h"
#include "ui_rateservicechangeview.h"
#include <QCloseEvent>
#include <chrono>
#include <thread>
#include <Helper.h>


std::shared_ptr<MessageManager> RateServiceChangeView::message_manager() const
{
    return message_manager_;
}

void RateServiceChangeView::setMessage_manager(const std::shared_ptr<MessageManager> &message_manager)
{
    message_manager_ = message_manager;
    if(message_manager)
    {
        message_manager_->setOnReadCB(std::bind(&RateServiceChangeView::onRead, this, std::placeholders::_1));
        message_manager_->allRates();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        message_manager_->allService();
    }
    ui->btnChangeRate->setEnabled(false);
    ui->btnChangeService->setEnabled(false);
}

void RateServiceChangeView::displayAllRates(network::AllRatesResponse &res)
{
    if(res.allrates_size() > 0)
    {
        for(int i = 0; i<res.allrates_size(); i++)
        {
            ListItemRate* rate = new ListItemRate(QString::fromStdString(res.allrates(i).about()));
            rate->rateInfo = res.allrates(i);
            ui->lwRates->addItem(rate);
        }
    }
}

void RateServiceChangeView::displayAllServices(network::AllServicesResponse &res)
{
    if(res.allservices_size() > 0)
    {
        for(int i = 0; i<res.allservices_size(); i++)
        {
            ListItemService* rate = new ListItemService(QString::fromStdString(res.allservices(i).about()));
            rate->serviceInfo = res.allservices(i);
            ui->lwServices->addItem(rate);
        }
    }
}

void RateServiceChangeView::ratePayment(network::RatePaymentResponse &res)
{
    ui->lbResponse->setText("Rate: " + QString::fromStdString(res.message_res()));
}

void RateServiceChangeView::servicePayment(network::ServicePaymentResponse &res)
{
    ui->lbResponse->setText("Service: " + QString::fromStdString(res.message_res()));
}

void RateServiceChangeView::readData(std::string str)
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
    case network::MO_SERVICE_PAYMENT:
    {
        network::ServicePaymentResponse res = response.servicepaymentresponse();
        servicePayment(res);
        break;
    }
    case network::MO_RATE_PAYMENT:
    {
        network::RatePaymentResponse res = response.ratepaymentresponse();
        ratePayment(res);
        break;
    }
    }
}

void RateServiceChangeView::closeEvent(QCloseEvent *event)
{
    emit onClose();
    event->accept();
}

RateServiceChangeView::RateServiceChangeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RateServiceChangeView)
{
    ui->setupUi(this);
    connect(this, SIGNAL(onReadData(std::string)), this, SLOT(readData(std::string)));
}

RateServiceChangeView::~RateServiceChangeView()
{
    delete ui;
}

void RateServiceChangeView::onRead(const network::ResponseContext &response)
{
    emit onReadData(response.SerializeAsString());
}

void RateServiceChangeView::on_lwRates_itemClicked(QListWidgetItem *item)
{
    ListItemRate* lir = static_cast<ListItemRate*>(item);
    ui->tbRateInfo->setText(QString::fromStdString(Helper::beautyRateInfo(lir->rateInfo)));
    ui->btnChangeRate->setEnabled(true);
}

void RateServiceChangeView::on_lwServices_itemClicked(QListWidgetItem *item)
{
    ListItemService* lir = static_cast<ListItemService*>(item);
    ui->tbServiceInfo->setText(QString::fromStdString(Helper::beautyServiceInfo(lir->serviceInfo)));
    ui->btnChangeService->setEnabled(true);
}

void RateServiceChangeView::on_btnChangeRate_clicked()
{
    ListItemRate* lir = static_cast<ListItemRate*>(ui->lwRates->currentItem());
    message_manager_->ratePayment(lir->rateInfo.id());
}

void RateServiceChangeView::on_btnChangeService_clicked()
{
    ListItemService* lir = static_cast<ListItemService*>(ui->lwServices->currentItem());
    message_manager_->servicePayment(lir->serviceInfo.id());
}
