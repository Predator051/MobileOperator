#include "ClientView.h"
#include "ui_clientview.h"
#include <QCloseEvent>

ClientView::ClientView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientView)
{
    ui->setupUi(this);
}

ClientView::~ClientView()
{
    delete ui;
}

void ClientView::closeEvent(QCloseEvent *event)
{
    emit onClose();
    event->accept();
}
