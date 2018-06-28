#include "eemaincontroller.h"

#include "EESDK/eesdk.h"
#include "EESDK/eefileloader.h"
#include "EESDK/eesignincontroller.h"

#include "EEWidget/eestartwindow.h"
#include "EEWidget/eecivicloginin.h"
#include "EEWidget/eelogin.h"
#include "EEWidget/eequitdialog.h"
#include "EEWidget/eemessagebox.h"
#include "EEWidget/eetrayicon.h"
#include "EEWidget/eesyncronizationfolderform.h"

#include "EEDataManager/eedatamanager.h"
#include "eesettingsclass.h"
#include "globalconstants.h"

#include <QMenu>
#include <QDebug>
#include <QCloseEvent>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>

EEMainController::EEMainController(QObject *parent) :
    QObject{parent},
    mSdk{new EESDK()},
    mSignInController{new EESignInController(mSdk)},
    mLoader{nullptr},
    mDataManager{nullptr},
    mTrayIcon{new EETrayIcon},
    mStartWindow{new EEStartWindow(mSdk)},
    mCivicLogin{new EECivicLoginIn(mSdk)},
    mLogin{new EELogin(mSdk)},
    mSyncronizationForm{new EESyncronizationFolderForm(mSdk)},
    mQuitDialog{new EEQuitDialog}
{
    connect(mTrayIcon, SIGNAL(signIn()), mStartWindow, SLOT(show()));
    connect(mTrayIcon, SIGNAL(signOut()), this, SLOT(signOut()));
    connect(mTrayIcon, SIGNAL(showChooseFolder()),this, SLOT(chooseFolderClicked()));
    connect(mTrayIcon, SIGNAL(closeProgram()), this, SLOT(closeProgramClicked()));

    connect(mStartWindow, &EEStartWindow::userLoggingIn, this, [this]() {
        mSignInController->setState(CurrentLoginState::LoggingIn);
        mCivicLogin->uploadPage();
    });

    connect(mStartWindow, &EEStartWindow::userRegistrating, this, [this]() {
        mSignInController->setState(CurrentLoginState::Registrating);
        mCivicLogin->uploadPage();
    });

    connect(mSignInController, SIGNAL(userLoggedIn()), this, SLOT(userLoggedIn()));

    connect(mCivicLogin, SIGNAL(receivedToken(QString)), mSignInController, SLOT(civicTokenReceived(QString)));
    connect(mSyncronizationForm, SIGNAL(newFolderChoosed(QString)), this, SLOT(anotherFolderChoosed(QString)));

    connect(mSdk, SIGNAL(requestError(EEError,QString)), this, SLOT(errorMessageBox(EEError,QString)));
}

EEMainController::~EEMainController()
{
    delete mQuitDialog;
    delete mTrayIcon;
    delete mSdk;
    delete mLogin;
    delete mCivicLogin;
    delete mStartWindow;

    if (mLoader != nullptr) {
        delete mLoader;
    }
    if (mDataManager != nullptr) {
        delete mDataManager;
    }
}

/**
 * @brief EEMainController::checkForAutosignin
 * Check is user has already been signed in.
 * If yes - doesn't show log in window and do direct signing in
 */
bool EEMainController::checkForAutosignin()
{
    bool rAutosignin = true;
    if (EESettingsClass::getInstance().getSettingsValue(SettingsOptions::Remember).toInt()) {
        mTrayIcon->setSignedIn();
        QString lUserName = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::Email).toString();
        QString lPassword = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::Password).toString();

        if (!lUserName.isEmpty()) {
            if (!lPassword.isEmpty()) {
                mSdk->setCredentials(lUserName, lPassword);
                QString lPath = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::FolderName).toString();
                startDataSyncronization(lPath);
            } else {
                qDebug() << "Password is empty!";
            }
        } else {
            qDebug() << "User name is empty!";
        }
    } else {
        rAutosignin = false;
    }

    return rAutosignin;
}

/**
 * @brief EEMainController::syncronizationStatus
 * Check is syncronization process is active, using timer,
 * and show warning message box if needed
 * @return
 */
bool EEMainController::syncronizationStatus()
{
    bool lIsSyncronization = false;
    if (mDataManager != nullptr) {
        if (!mDataManager->isTimerRunning()) {
            lIsSyncronization = true;
            QString lText = "Data synchronisation process is running. "
                            "Please wait a few seconds until it ends and try again";

            EEMessageBox lMessageBox;
            lMessageBox.setText(lText);
            lMessageBox.setStandardButtons(EEMessageBox::Ok);
            lMessageBox.exec();
        }
    }

    return lIsSyncronization;
}
/**
 * @brief EEMainController::startSignInWithCivic
 * Try to auto sign in
 * If no, show start window, which provide possibility to choose register or log in
 */
void EEMainController::startSignInWithCivic()
{
    qDebug() << typeid(*this).name() << __FUNCTION__;

    if (!checkForAutosignin()) {
        mStartWindow->show();
    }
}
/**
 * @brief EEMainController::chooseFolderClicked
 * After "choose button" clicked
 * If syncronization is active - show warning message box
 */
void EEMainController::chooseFolderClicked()
{
    if (syncronizationStatus()) {
        return;
    }
    mSyncronizationForm->show();
}
/**
 * @brief EEMainController::closeProgramClicked
 * Show quit dialog
 * if syncronization is active - show warning message box
 */
void EEMainController::closeProgramClicked()
{
    if (syncronizationStatus()) {
        return;
    }
    mQuitDialog->show();
}

/**
 * @brief EEMainController::errorMessageBox
 * Custom warninng message box with text
 * @param error
 * @param result
 */
void EEMainController::errorMessageBox(EEError error, QString result)
{
    (void)result;
    EEMessageBox lMessageBox;
    lMessageBox.setIcon(QMessageBox::Warning);
    lMessageBox.setText(error.message());
    lMessageBox.setStandardButtons(EEMessageBox::Ok);
    lMessageBox.exec();
}

/**
 * @brief EEMainController::anotherFolderChoosed
 * If another folder has been choosed - remove previous json
 * @param path
 */
void EEMainController::anotherFolderChoosed(QString path)
{
    QFile lJsonPath(QCoreApplication::applicationDirPath() + "/" + GlobalData::gTemporaryFolder + "/" + GlobalData::gJsonName);

    qDebug() << lJsonPath.fileName();
    if (lJsonPath.exists()) {
        if (lJsonPath.remove()) {
            qDebug() << "Previous json has been deleted" << lJsonPath.fileName();
        }
    }

    startDataSyncronization(path);
}
/**
 * @brief EEMainController::signOut
 * if user click sing out - stop timer for status check and set button as sign in
 */
void EEMainController::signOut()
{
    //check is syncronization process is active now
    if (syncronizationStatus()) {
        return;
    }

    if (mDataManager != nullptr) {
        mDataManager->stopTimerForDataCheck();
    }
    mTrayIcon->setSignedOut();
    mStartWindow->show();
}
/**
 * @brief EEMainController::closeProgram
 */
void EEMainController::closeProgram()
{
    EEQuitDialog lDialog;

    if (lDialog.exec() == QDialog::Accepted) {
        QApplication::quit();
    }
}
/**
 * @brief EEMainController::startDataSyncronization
 * Initialize loader and data manager,
 * check syncronization of data and start timer
 * @param path
 */
void EEMainController::startDataSyncronization(QString path)
{
    qDebug() << typeid(*this).name() << __FUNCTION__;
    qDebug() << "Path: " << path;

    QDir lDirectory(path);
    if (!lDirectory.exists()) {
        if (!lDirectory.mkpath(".")) {
            qDebug() << "Folder wasn't created!";
        }
    }

    if (mLoader != nullptr) {
        delete mLoader;
    }

    mLoader = new EEFileLoader{mSdk};

    if (mDataManager != nullptr) {
        delete mDataManager;
    }
    mDataManager = new EEDataManager{mSdk, mLoader, path};
    mDataManager->start();
}

void EEMainController::stopDataSyncronization()
{
    mDataManager->stopTimerForDataCheck();

    EESettingsClass::getInstance().setSettingsValue(SettingsOptions::Remember, false);
}

/**
 * @brief EEMainController::showInformationMessageBox
 * Provides possibility to choose default or custom folder for syncronization
 */
void EEMainController::userLoggedIn()
{
    mTrayIcon->setSignedIn();

    QString lPath = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::FolderName).toString();

    if (lPath.isEmpty()) {
        mSyncronizationForm->resetDefaultPath();
        lPath = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::FolderName).toString();
    }

    QString lText = "Folder for syncronization is:\n" + lPath;

    EEMessageBox lMessageBox;
    lMessageBox.setText(lText);
    lMessageBox.setInformativeText("Do you want to keep using this folder?");
    lMessageBox.setStandardButtons(EEMessageBox::Yes | EEMessageBox::No);
    int lResult = lMessageBox.exec();

    switch (lResult) {
      case EEMessageBox::Accepted:
          startDataSyncronization(lPath);
          break;
      case EEMessageBox::Rejected:
          mSyncronizationForm->show();
          break;
      default:
          qDebug() << "Buttons wasn't pressed!";
          break;
    }
}
