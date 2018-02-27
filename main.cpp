#include "eemaincontroller.h"

#include <QApplication>
#include <QPixmap>
#include <QtWidgets/QSystemTrayIcon>

#include "EESDK/eesdk.h"
#include "EEDataSync/eenodeparser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap lPixmap(32,32);
    lPixmap.load (":/resources/logo.png");

    qDebug() << EENodeParser::initNode("123", "Today");

    a.setWindowIcon(QIcon(lPixmap));

    QSystemTrayIcon *lTrayIcon = new QSystemTrayIcon(QIcon(lPixmap));
    lTrayIcon->setVisible(true);
    lTrayIcon->showMessage(QObject::tr("Hello"),
                          QObject::tr("All your files are in XCloud in your home dir"),
                          QSystemTrayIcon::Information, 1000);

    //class that handles all evants from tray menu
    EEMainController w;
    QObject::connect(lTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), &w, SLOT(handleClick(QSystemTrayIcon::ActivationReason)));

    return a.exec();
}
