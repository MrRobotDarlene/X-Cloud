#ifndef EETRAYICON_H
#define EETRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QVector>


class QMenu;
class QAction;

/**
 * @brief The EETrayIcon class
 * Class - wrapper for qt tray icon.
 * Handle emiting of the signals after clicking on some actions
 */
class EETrayIcon : public QObject
{
    Q_OBJECT
public:
    explicit EETrayIcon(QObject *parent = 0);
    ~EETrayIcon();

    QMenu *trayMenu() const;
    QSystemTrayIcon *trayIcon() const;
    void showTrayIconMessage() const;

    void setSignedIn();
    void setSignedOut();

signals:
    void trayIconAcivated(QSystemTrayIcon::ActivationReason);
    void signIn();
    void signOut();
    void showChooseFolder();
    void closeProgram();

public slots:
    void closeTray();
private slots:
    void signClicked();
private:
    QVector<QAction* >mListOfActions;

    QMenu *mMenu;
    QSystemTrayIcon *mTrayIcon;
    QAction *mActionActivate;
};

#endif // EETRAYICON_H
