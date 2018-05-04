#include "LehaStar.h"
#include "ui_LehaStar.h"
#include "GlobalParams.h"
#include "QMessageBox"
#include "Helper.h"
#include <QTimer>

LehaStar::LehaStar(std::shared_ptr<MessageManager> message_manager, QWidget *parent) :
    message_manager_(message_manager),
    QWidget(parent),
    ui(new Ui::LehaStar)
{
    ui->setupUi(this);
    if(message_manager)
        message_manager_->setOnErrorCB(std::bind(&LehaStar::onError, this, std::placeholders::_1));

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

void LehaStar::cannotConnectError()
{
    QMessageBox::critical(this, "Error", "Failure trying connect to network!");
    this->close();
}

void LehaStar::disconnectError()
{
    QMessageBox::critical(this, "Error", "You was disconnected! Restart application for connecting");
}

void LehaStar::on_testBtn_clicked()
{
    if(!ui->textEdit->toPlainText().isEmpty())
    {
        std::string str = ui->textEdit->toPlainText().toStdString();
        ByteBufferPtr buff = std::make_shared<ByteBuffer>(str.begin(), str.end());
        if(message_manager_)
            message_manager_->execute(buff);
    }
}

void LehaStar::updateTime()
{/*
    if(!message_manager_)
    {
        cannotConnectError();
    }*/
}
