#include "eemaincontroller.h"
#include "EESDK/eesdk.h"
#include "eelog.h"
#include "eefolderparsecontroller.h"
#include "eejsonbuilder.h"

#include <QApplication>
#include <QPixmap>
#include <QtWidgets/QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EELog::getInstance()->initialise();

    EEMainController mainController;
    mainController.startSignInWithCivic();

    return a.exec();
}
