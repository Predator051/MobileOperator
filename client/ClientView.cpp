#include "ClientView.h"
#include "ui_clientview.h"
#include <QCloseEvent>

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
        LOG_INFO("SET");
        message_manager_->setOnReadCB(std::bind(&ClientView::onRead, this, std::placeholders::_1));
        message_manager_->userPackage();
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
    tb->setText(aboutRate);
}

void ClientView::closeEvent(QCloseEvent *event)
{
    emit onClose();
    event->accept();
}

void ClientView::on_actionEXIT_2_triggered()
{
    this->close();
}

void ClientView::readData(std::string str)
{
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
    }
}
