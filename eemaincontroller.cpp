#include "eemaincontroller.h"

#include "EESDK/eesdk.h"
#include "EESDK/eefileloader.h"
#include "EESDK/eecryptowrapper.h".h"

#include "EEWidget/eelogin.h"
#include "EEWidget/eequitdialog.h"

#include "EEDataSync/eedatamanager.h"

#include <QMenu>
#include <QDebug>
#include <QCloseEvent>
#include <QApplication>

EEMainController::EEMainController(QObject *parent) :
    QObject(parent),
    mMenu{new QMenu},
    mSdk{new EESDK()},
    mLogin{new EELogin(mSdk)},
    mWrapper{new EECryptoWrapper()},
    mLoader{new EEFileLoader(mSdk, mWrapper)},
    mDataManager{new EEDataManager(mSdk, mLoader, mWrapper)}
{
    mMenu->addAction(tr("Sign in"), this, SLOT(showSignInForm()));
    mMenu->addAction(tr("Account Information"), this, SLOT(showAccountInformationForm()))->setEnabled(false);
    mMenu->addAction(tr("About X Cloud"), this, SLOT(showAboutRoamVUForm()));
    mMenu->addAction(tr("Report a Problem"), this, SLOT(showReportProblemForm()));
    mMenu->addAction(tr("Exit"), this, SLOT(closeProgram()));    
}
EEMainController::~EEMainController()
{
    delete mSdk;
    delete mLogin;
    delete mMenu;

    delete mDataManager;
}

void EEMainController::showSignInForm()
{
    mLogin->show();
}

void EEMainController::showAccountInformationForm()
{
}

void EEMainController::showAboutRoamVUForm()
{
}

void EEMainController::showReportProblemForm()
{
}

void EEMainController::loggedIn()
{
}

void EEMainController::closeProgram()
{
    EEQuitDialog lDialog;

    if (lDialog.exec() == QDialog::Accepted) {
        QApplication::quit();
    }
}
void EEMainController::handleClick(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger: {
        QSystemTrayIcon *lSender = (QSystemTrayIcon*)QObject::sender();
        mMenu->popup(QPoint(lSender->geometry().x(), lSender->geometry().y()));
    }
        break;
    }
}
