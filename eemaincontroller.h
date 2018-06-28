#ifndef EEMAINCONTROLLER_H
#define EEMAINCONTROLLER_H

#include <QObject>

#include "EESDK/EEContainers/eeerror.h"

class QCloseEvent;
class EESDK;
class EEDataManager;
class EEFileLoader;
class EESignInController;

class EETrayIcon;
class EEStartWindow;
class EECivicLoginIn;
class EELogin;
class EESyncronizationFolderForm;
class EEQuitDialog;


class EEMainController : public QObject
{
    Q_OBJECT
public:
    explicit EEMainController(QObject *parent = 0);
    ~EEMainController();

private:
    /**
     * @brief EEMainController::checkForAutosignin
     * Check is user has already been signed in.
     * If yes - doesn't show log in window and do direct signing in
     */
    bool checkForAutosignin();
    /**
     * @brief EEMainController::syncronizationStatus
     * Check is syncronization process is active, using timer,
     * and show warning message box if needed
     * @return
     */
    bool syncronizationStatus();
public slots:

    /**
     * @brief EEMainController::startSignInWithCivic
     * Try to auto sign in
     * If no, show start window, which provide possibility to choose register or log in
     */
    void startSignInWithCivic();

    /**
     * @brief EEMainController::chooseFolderClicked
     * After "choose button" clicked
     * If syncronization is active - show warning message box
     */
    void chooseFolderClicked();

    /**
     * @brief EEMainController::chooseFolderClicked
     * After "choose button" clicked
     * If syncronization is active - show warning message box
     */
    void closeProgramClicked();

private slots:
    /**
     * @brief EEMainController::errorMessageBox
     * Custom warninng message box with text
     * @param error
     * @param result
     */
    void errorMessageBox(EEError error, QString result);

    /**
     * @brief EEMainController::anotherFolderChoosed
     * If another folder has been choosed - remove previous json
     * @param path
     */
    void anotherFolderChoosed(QString path);
    /**
     * @brief EEMainController::signOut
     * if user click sing out - stop timer for status check and set button as sign in
     */
    void signOut();
    /**
     * @brief EEMainController::closeProgram
     */
    void closeProgram();
    /**
     * @brief EEMainController::startDataSyncronization
     * Initialize loader and data manager,
     * check syncronization of data and start timer
     * @param path
     */
    void startDataSyncronization(QString path);
    /**
     * @brief EEMainController::stopDataSyncronization
     * Stops loader and data manager,
     * check syncronization of data and stops timer
     * @param path
     */
    void stopDataSyncronization();
    /**
     * @brief EEMainController::userLoggedIn
     * Provides possibility to choose default or custom folder for syncronization
     */
    void userLoggedIn();
private:
    EESDK *mSdk;
    EESignInController *mSignInController;
    EEFileLoader *mLoader;
    EEDataManager *mDataManager;

    EETrayIcon *mTrayIcon;

    EEStartWindow *mStartWindow;
    EECivicLoginIn *mCivicLogin;
    EELogin *mLogin;
    EESyncronizationFolderForm *mSyncronizationForm;
    EEQuitDialog *mQuitDialog;
};

#endif // EEMAINCONTROLLER_H
