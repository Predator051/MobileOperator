#include "Worker.h"
#include "define.h"
#include "MessageManager.h"
#include "LehaStar.h"
#include <QApplication>
#include "Config/ConfigManager.h"
#include "Config/GlobalParams.h"

int showWindow(std::shared_ptr<MessageManager> messManag)
{
    messManag->start();
}

int main(int argc, char *argv[])
{
//   MessageManager mng("127.0.0.1", "2222");
    ConfigManager configManager;
    if(!configManager.readConfigFIle("config.cfg"))
    {
        LOG_ERR("Faliure to try reading config file")
                return EXIT_FAILURE;
    }
    configManager.parseConfigs();

    QApplication app(argc, argv);

    std::shared_ptr<MessageManager> messManager = std::make_shared<MessageManager>(GlobalsParams::getListenChannelAddress()
                                                                                   , std::to_string(GlobalsParams::getListenChannelPort()));
    LehaStar lehaStar(messManager);

    std::thread windowThread(showWindow, messManager);
    windowThread.detach();
//    mng.start();

    lehaStar.show();

    return app.exec();
}

