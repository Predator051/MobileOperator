#include "AdminView.h"
#include "ui_adminview.h"
#include <QCloseEvent>

AdminView::AdminView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminView)
{
    ui->setupUi(this);
}

AdminView::~AdminView()
{
    delete ui;
}

void AdminView::closeEvent(QCloseEvent *event)
{
    emit onClose();
    event->accept();
}
