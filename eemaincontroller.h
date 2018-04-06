#ifndef EEMAINCONTROLLER_H
#define EEMAINCONTROLLER_H

#include <QObject>

class QCloseEvent;
class EESDK;
class EEDataManager;
class EEFileLoader;
class EECryptoWrapper;


class EETrayIcon;
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
    EETrayIcon *mTrayIcon;

    EELogin *mLogin;
    EESyncronizationFolderForm *mSyncronizationForm;
    EEQuitDialog *mQuitDialog;


    EECryptoWrapper *mWrapper;
    EEFileLoader *mLoader;
    EEDataManager *mDataManager;
};

#endif // EEMAINCONTROLLER_H
