#include "Worker.h"
#include "define.h"
#include "MessageManager.h"
#include "LehaStar.h"
#include <QApplication>

int showWindow(std::shared_ptr<MessageManager> messManag)
{
    messManag->start();
}

int main(int argc, char *argv[])
{
//   MessageManager mng("127.0.0.1", "2222");

    QApplication app(argc, argv);

    std::shared_ptr<MessageManager> messManager = std::make_shared<MessageManager>("127.0.0.1", "2222");
    LehaStar lehaStar(messManager);

    std::thread windowThread(showWindow, messManager);
    windowThread.detach();
//    mng.start();

    lehaStar.show();

    return app.exec();
}

