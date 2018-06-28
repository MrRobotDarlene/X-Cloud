#ifndef EELOGIN_H
#define EELOGIN_H

#include "eewidget.h"

#include <QWidget>

class EERegister;

namespace Ui {
class EELogin;
}
/**
 * @brief The EELogin class
 * Login form
 */
class EELogin : public EEWidget
{
    Q_OBJECT

public:
    explicit EELogin(EESDK *sdk, QWidget *parent = 0);
    ~EELogin();

private:
    bool isEmailValid();

signals:
    void userLoggedIn();
private slots:
    /**
     * @brief EELogin::on_pushButtonLogin_clicked
     * Try to log in.
     * If seems as succesful - ping server with buckets request
     */
    void on_pushButtonLogin_clicked();

    /**
     * @brief EELogin::on_pushButtonRegister_clicked
     * Call registration window
     */
    void on_pushButtonRegister_clicked();

    void on_lineEditEmail_textChanged(const QString &arg1);

    void on_lineEditPassword_textChanged(const QString &arg1);

    /**
     * @brief EELogin::requestError
     * Calls in case of error responce from server
     * @param err
     * @param method
     */
    void requestError(EEError, QString);
    /**
     * @brief EELogin::serverPinged
     * After logging in - ping server with request for buckets to check corectness of signing in
     * @param buckets
     */
    void serverPinged(QList<EEBucket *>);

private:
    Ui::EELogin *ui;
    EERegister *mRegister;
};

#endif // EELOGIN_H
