#include "LehaStar.h"
#include "ui_LehaStar.h"
#include "GlobalParams.h"
#include "QMessageBox"
#include "Helper.h"
#include <QTimer>
#include "Cache/CacheManager.h"

LehaStar::LehaStar(std::shared_ptr<MessageManager> message_manager, QWidget *parent) :
    message_manager_(message_manager),
    QWidget(parent),
    ui(new Ui::LehaStar)
{
    ui->setupUi(this);
    if(message_manager)
    {
        message_manager_->setOnErrorCB(std::bind(&LehaStar::onError, this, std::placeholders::_1));
        message_manager->setOnReadCB(std::bind(&LehaStar::onRead, this, std::placeholders::_1));
        message_manager->setOnConnected(std::bind(&LehaStar::onConnected, this));
    }

    timer_ = new QTimer();
    timer_->setInterval(1000);
    connect(timer_, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer_->start();
}

LehaStar::~LehaStar()
{
    delete ui;
}

void LehaStar::onError(ClientError error)
{
    switch (error) {
    case ClientError::status_cannot_connect:
        cannotConnectError();
        break;
    case ClientError::status_disconnect:
        disconnectError();
        break;
    default:
        break;
    }
}

void LehaStar::onRead(const network::ResponseContext & response)
{
    LOG_INFO("Response from server: " << response.error_code());

    switch (response.message_type_()) {
    case network::MO_REGISTER:
    {
        network::RegisterMessageResponse regRes = response.register_response();
        userRegister(regRes);
        break;
    }
    case network::MO_AUTH:
    {
        network::AuthMessageResponse authRes = response.auth_response();
        message_manager_->setSessionInfo(response.session_info());
        userAuth(authRes, message_manager_->sessionInfo());
        break;
    }
    case network::MO_USER_STATUS:
    {
        message_manager_->setSessionInfo(response.session_info());
        userStatus(message_manager_->sessionInfo());
        break;
    }
    }
}

void LehaStar::onConnected()
{
    LOG_INFO("Connected!");
    message_manager_->userStatus();
}

void LehaStar::cannotConnectError()
{
    QMessageBox::critical(this, "Error", "Failure trying connect to network!");
    this->close();
}

void LehaStar::disconnectError()
{
    QMessageBox::critical(this, "Error", "You was disconnected! Restart application for connecting");
}

void LehaStar::userRegister(const network::RegisterMessageResponse &response)
{
    ui->registerLabel->setText(QString::fromStdString(response.messagetext()));
    LOG_INFO(response.messagetext());
}

void LehaStar::userAuth(const network::AuthMessageResponse &authMessage, const network::SessionInfo &sessionInfo)
{
    ui->registerLabel_2->setText(QString::fromStdString(authMessage.server_message()));
    LOG_INFO(authMessage.server_message());
    LOG_INFO("Role: [" << sessionInfo.role() << "]:" << sessionInfo.session_id());


    if(authMessage.status())
    {

        CacheManager::instance("").saveSession(sessionInfo.session_id());

        KeyValueMap cache;
        cache["session"] = sessionInfo.session_id();
        cache["user_id"] = std::to_string(sessionInfo.userid());
        cache["login"] = sessionInfo.login();

        CacheManager::instance(sessionInfo.session_id()).makeCache(cache);

        GlobalsParams::getSessionInfo() = sessionInfo;
        switch (sessionInfo.role()) {
        case 0:
            clientView_ = std::make_shared<ClientView>();
            connect(clientView_.get(), SIGNAL(onClose()), this, SLOT(logout()));
            clientView_->setAttribute(Qt::WA_DeleteOnClose, true);
            clientView_->show();
            break;
        case 1:
            adminView_ = std::make_shared<AdminView>();
            connect(adminView_.get(), SIGNAL(onClose()), this, SLOT(logout()));
            adminView_->setAttribute(Qt::WA_DeleteOnClose, true);
            adminView_->show();
            break;
        default:
            break;
        }
    }
}

void LehaStar::userStatus(const network::SessionInfo &sessionInfo)
{
    if(sessionInfo.session_id().empty())
    {
        return;
    }

    CacheManager::instance("").saveSession(sessionInfo.session_id());

    switch (sessionInfo.role()) {
    case 0:
        clientView_ = std::make_shared<ClientView>();
        connect(clientView_.get(), SIGNAL(onClose()), this, SLOT(logout()));
        clientView_->setAttribute(Qt::WA_DeleteOnClose, true);
        clientView_->show();
        break;
    case 1:
        adminView_ = std::make_shared<AdminView>();
        connect(adminView_.get(), SIGNAL(onClose()), this, SLOT(logout()));
        adminView_->setAttribute(Qt::WA_DeleteOnClose, true);
        adminView_->show();
        break;
    default:
        break;
    }
}

void LehaStar::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    QCoreApplication::processEvents();
}

void LehaStar::on_testBtn_clicked()
{
    if(!ui->loginTE->toPlainText().isEmpty() && !ui->passwordTE->toPlainText().isEmpty())
    {
        std::string login = ui->loginTE->toPlainText().toStdString();
        std::string password = ui->passwordTE->toPlainText().toStdString();
        message_manager_->userAuth(login, password);
    }
}

void LehaStar::updateTime()
{/*
    if(!message_manager_)
    {
        cannotConnectError();
    }*/
}

void LehaStar::on_testBtn_2_clicked()
{
    if(!ui->loginUPTE->toPlainText().isEmpty() && !ui->passwordUPTE->toPlainText().isEmpty())
    {
        std::string login = ui->loginUPTE->toPlainText().toStdString();
        std::string password = ui->passwordUPTE->toPlainText().toStdString();
        message_manager_->createUser(login, password);
    }
}

void LehaStar::logout()
{
    LOG_INFO("Close event!");
    //message_manager_->logout();
    //message_manager_->sessionInfo().Clear();
}
