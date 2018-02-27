#ifndef EEMAINCONTROLLER_H
#define EEMAINCONTROLLER_H

#include <QObject>
#include <QtWidgets/QSystemTrayIcon>

class QCloseEvent;
class QMenu;
class EESDK;
class EEDataManager;
class EEFileLoader;
class EECryptoWrapper;

class EELogin;

class EEMainController : public QObject
{
    Q_OBJECT
public:
    explicit EEMainController(QObject *parent = 0);
    ~EEMainController();

private slots:
    void showSignInForm();
    void showAccountInformationForm();
    void showAboutRoamVUForm();
    void showReportProblemForm();
    void loggedIn();
    void closeProgram();
public slots:
    void handleClick(QSystemTrayIcon::ActivationReason);
private:
    QMenu *mMenu;
    EESDK *mSdk;

    EELogin *mLogin;

    EECryptoWrapper *mWrapper;
    EEFileLoader *mLoader;
    EEDataManager *mDataManager;
};

#endif // EEMAINCONTROLLER_H
