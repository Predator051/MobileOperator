#include "Worker.h"
#include "Server.h"
#include "RRManager.h"
#include "Config/ConfigManager.h"
#include "Config/GlobalParams.h"

int main(int argc, char *argv[])
{

    ConfigManager conf;
    if(!conf.readConfigFIle("config.cfg"))
    {
        return EXIT_FAILURE;
    }
    conf.parseConfigs();

    Server server(GlobalsParams::getListenChannelAddress(), GlobalsParams::getListenChannelPort());

    RRManager chatManager(server);

    server.start();

    Worker::instance()->start();

    LOG_INFO("Threads started!!!");

    Worker::instance()->join();

    LOG_INFO("Threads FINISHED!!!");


    return 0;
}

