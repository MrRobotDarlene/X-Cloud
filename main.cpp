#include "eemaincontroller.h"

#include <QApplication>
#include <QPixmap>
#include <QtWidgets/QSystemTrayIcon>

#include "EESDK/eesdk.h"
#include "eelog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EELog::getInstance()->initialise();

    EEMainController w;

    return a.exec();
}
