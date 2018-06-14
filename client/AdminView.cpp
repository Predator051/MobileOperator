#include "AdminView.h"
#include "ui_adminview.h"
#include <QCloseEvent>
#include "Helper.h"
#include <thread>
#include <chrono>
#include <QVector>
#include <View/ListItem.h>

network::RateStatisticsResponse AdminView::rateStatisticResponse;

AdminView::AdminView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminView)
{
    qRegisterMetaType<std::string>("std::string");

    ui->setupUi(this);
    connect(this, SIGNAL(onReadData(std::string)), this, SLOT(readData(std::string)));
    ui->tabWidget->setCurrentIndex(0);
    ui->pushButton_2->setEnabled(false);


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

void AdminView::displayRateStatistics()
{
        double a = 0;
        double b =  100;
        double h = 1;

        int N=(b-a)/h + 2; //Вычисляем количество точек, которые будем отрисовывать
        QVector<double> x(N), y(N); //Массивы координат точек

        //Вычисляем наши данные
        int i=0;
        for (double X=a; X<=b; X+=h)//Пробегаем по всем точкам
        {
            x[i] = X;
            y[i] = X;//Формула нашей функции
            i++;
        }

        ui->rateCountPlot->clearGraphs();//Если нужно, но очищаем все графики
        //Добавляем один график в widget
        ui->rateCountPlot->addGraph();
        //Говорим, что отрисовать нужно график по нашим двум массивам x и y
        ui->rateCountPlot->graph(0)->setData(x, y);

        //Подписываем оси Ox и Oy
        ui->rateCountPlot->xAxis->setLabel("x");
        ui->rateCountPlot->yAxis->setLabel("y");

        //Установим область, которая будет показываться на графике
        ui->rateCountPlot->xAxis->setRange(a, b);//Для оси Ox

        //Для показа границ по оси Oy сложнее, так как надо по правильному
        //вычислить минимальное и максимальное значение в векторах
        double minY = y[0], maxY = y[0];
        for (int i=1; i<N; i++)
        {
            if (y[i]<minY) minY = y[i];
            if (y[i]>maxY) maxY = y[i];
        }
        ui->rateCountPlot->yAxis->setRange(minY, maxY);//Для оси Oy

        //И перерисуем график на нашем widget
        ui->rateCountPlot->replot();
}

void AdminView::rateStatistics(network::RateStatisticsResponse &response)
{
    ui->lwRatesStats->clear();
    if(response.rate_month_size() > 0)
    {
        std::map<uint64_t, ListItemRate*> maps;
        for(int i = 0; i<response.rate_month_size(); i++)
        {
            if(maps.find(response.rate_month(i).rate_info().id()) == maps.end())
            {
                ListItemRate* lir = new ListItemRate(QString::fromStdString(response.rate_month(i).rate_info().about()));
                maps[response.rate_month(i).rate_info().id()] = lir;
                lir->statistics.push_back(response.rate_month(i));
                ui->lwRatesStats->addItem(lir);
            }
            else
            {
                maps[response.rate_month(i).rate_info().id()]->statistics.push_back(response.rate_month(i));
            }
        }
    }
}

void AdminView::serviceStatistics(network::ServiceStatisticsResponse &response)
{
    ui->lwServiceStats->clear();
    if(response.service_month_size() > 0)
    {
        std::map<uint64_t, ListItemService*> maps;
        for(int i = 0; i<response.service_month_size(); i++)
        {
            if(maps.find(response.service_month(i).service_info().id()) == maps.end())
            {
                ListItemService* lir = new ListItemService(QString::fromStdString(response.service_month(i).service_info().name()));
                maps[response.service_month(i).service_info().id()] = lir;
                lir->statistics.push_back(response.service_month(i));
                ui->lwServiceStats->addItem(lir);
            }
            else
            {
                maps[response.service_month(i).service_info().id()]->statistics.push_back(response.service_month(i));
            }
        }
    }
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
    case network::MO_RATE_STATISTICS:
    {
        network::RateStatisticsResponse res = response.rate_statistics();
        rateStatistics(res);
        break;
    }
    case network::MO_SERVICE_STATISTICS:
    {
        network::ServiceStatisticsResponse res = response.service_statistics();
        serviceStatistics(res);
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
    ui->pushButton_2->setEnabled(true);
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

}

void AdminView::on_tabWidget_tabBarClicked(int index)
{
    LOG_INFO(index);
    switch (index) {

    case 1:
    {
        message_manager_->rateStatistics();

        ui->serviceCountPlot->xAxis->setLabel("month");
        ui->serviceCountPlot->yAxis->setLabel("count usage");
        break;
    }
    case 2:
    {
        message_manager_->serviceStatistics();

        ui->serviceCountPlot->xAxis->setLabel("month");
        ui->serviceCountPlot->yAxis->setLabel("count usage");
        break;
    }
    }
}

void AdminView::on_lwRatesStats_itemClicked(QListWidgetItem *item)
{
    ListItemRate* lir = static_cast<ListItemRate*>(item);

    double a = 0;
    double b =  100;
    double h = 1;

    int N=lir->statistics.size(); //Вычисляем количество точек, которые будем отрисовывать
    QVector<double> x(12), y(12); //Массивы координат точек
    //Вычисляем наши данные
    int i=0;
    for(int month = 0; month < 12; month++)
    {
        x[i] = month;

        for(network::RateStatisticsResponse_MonthCount rsrmc: lir->statistics)
        {
            if(rsrmc.month() == month)
            {
                y[i] = rsrmc.count_rate();
            }
        }

        i++;
    }

    ui->rateCountPlot->clearGraphs();
    ui->rateCountPlot->addGraph();
    ui->rateCountPlot->graph(0)->setData(x, y);

    ui->rateCountPlot->xAxis->setLabel("month");
    ui->rateCountPlot->yAxis->setLabel("count usage");

    double minY = y[0], maxY = y[0];
    for (int i=1; i<N; i++)
    {
        if (y[i]>maxY) maxY = y[i];
    }

    //Установим область, которая будет показываться на графике
    ui->rateCountPlot->xAxis->setRange(0, 12);//Для оси Ox
    ui->rateCountPlot->yAxis->setRange(0, 10);//Для оси Oy

    //И перерисуем график на нашем widget
    ui->rateCountPlot->replot();
}

void AdminView::on_lwServiceStats_itemClicked(QListWidgetItem *item)
{
    ListItemService* lir = static_cast<ListItemService*>(item);

    double a = 0;
    double b =  100;
    double h = 1;

    int N=lir->statistics.size(); //Вычисляем количество точек, которые будем отрисовывать
    QVector<double> x(12), y(12); //Массивы координат точек
    //Вычисляем наши данные
    int i=0;
    for(int month = 1; month < 12; month++)
    {
        x[i] = month;

        for(network::ServiceStatisticsResponse_MonthCount rsrmc: lir->statistics)
        {
            if(rsrmc.month() == month)
            {
                y[i] = rsrmc.count_service();
            }
        }

        i++;
    }

    ui->serviceCountPlot->clearGraphs();
    ui->serviceCountPlot->addGraph();
    ui->serviceCountPlot->graph(0)->setData(x, y);

    ui->serviceCountPlot->xAxis->setLabel("month");
    ui->serviceCountPlot->yAxis->setLabel("count usage");

    double minY = y[0], maxY = y[0];
    for (int i=1; i<N; i++)
    {
        if (y[i]>maxY) maxY = y[i];
    }

    //Установим область, которая будет показываться на графике
    ui->serviceCountPlot->xAxis->setRange(0, 12);//Для оси Ox
    ui->serviceCountPlot->yAxis->setRange(0, 10);//Для оси Oy

    //И перерисуем график на нашем widget
    ui->serviceCountPlot->replot();
}

void AdminView::on_actionLOGOUT_triggered()
{
    emit onClose(ExitAction::LOGOUT);
    this->hide();
}

void AdminView::on_actionEXIT_triggered()
{
    this->close();
}
