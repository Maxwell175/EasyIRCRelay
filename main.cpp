#include <QCoreApplication>

#include "mainworker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MainWorker* Worker = new MainWorker();
    Worker->StartMainWorker();

    return a.exec();
}
