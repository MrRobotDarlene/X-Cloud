#include "eetrayicon.h"

#include <QPixmap>
#include <QMenu>
#include <QAction>
#include <QDebug>

#define TEXT_SIGNIN     "Sign In"
#define TEXT_SIGNOUT    "Sign Out"

EETrayIcon::EETrayIcon(QObject *parent)
    : QObject(parent),
      mMenu{new QMenu},
      mTrayIcon{new QSystemTrayIcon}
{
    mActionActivate = new QAction(tr(TEXT_SIGNIN), this);
    connect(mActionActivate, SIGNAL(triggered(bool)), this, SLOT(signClicked()));

    QAction *actionAccountInformation = new QAction(tr("Choose folder..."), this);
    connect(actionAccountInformation, SIGNAL(triggered(bool)), this, SIGNAL(showChooseFolder()));

    QAction *actionExit = new QAction(tr("Exit"), this);

    connect(actionExit, SIGNAL(triggered(bool)), this, SIGNAL(closeProgram()));

    mListOfActions.push_back(mActionActivate);
    mListOfActions.push_back(actionAccountInformation);
    mListOfActions.push_back(actionExit);

    for (auto action : mListOfActions) {
        mMenu->addAction(action);
    }
    setSignedOut();


    //creating icon for tray
    QPixmap lPixmap(32,32);
    lPixmap.load (":/resources/xcloud.png");

    mTrayIcon->setIcon(QIcon(lPixmap));
    mTrayIcon->setContextMenu(mMenu);
    mTrayIcon->show();

    QObject::connect(mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SIGNAL(trayIconAcivated(QSystemTrayIcon::ActivationReason)));
}

EETrayIcon::~EETrayIcon()
{
    //qDeleteAll(mListOfActions);
    delete mMenu;
    delete mTrayIcon;
}

QMenu *EETrayIcon::trayMenu() const
{
    return mMenu;
}

QSystemTrayIcon *EETrayIcon::trayIcon() const
{
    return mTrayIcon;
}

void EETrayIcon::showTrayIconMessage() const
{
    mTrayIcon->showMessage(QObject::tr("Hello"),
                           QObject::tr("All your files are in X Cloud in your home dir"),
                           QSystemTrayIcon::Information, 1000);
}

void EETrayIcon::setSignedIn()
{
    mListOfActions.at(0)->setEnabled(true);
    mListOfActions.at(1)->setEnabled(true);
    mListOfActions.at(2)->setEnabled(true);

    mActionActivate->setText(tr(TEXT_SIGNOUT));
}

void EETrayIcon::setSignedOut()
{
    mListOfActions.at(0)->setEnabled(true);
    mListOfActions.at(1)->setEnabled(false);
    mListOfActions.at(2)->setEnabled(true);

    mActionActivate->setText(tr(TEXT_SIGNIN));

}

void EETrayIcon::closeTray()
{
    mTrayIcon->hide();
}

void EETrayIcon::signClicked()
{
    if (mActionActivate->text() == tr(TEXT_SIGNIN)) {
        emit signIn();

    } else if (mActionActivate->text() == tr(TEXT_SIGNOUT)) {
        emit signOut();
    }
}
