#include <QCoreApplication>

#include "eesdktest.h"
#include "EEDataSync/eedatamanager.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    EESDKTest test;
    test.startTests();

    return a.exec();
}
