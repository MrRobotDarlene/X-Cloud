#ifndef EELOGIN_H
#define EELOGIN_H

#include "eewidget.h"

#include <QWidget>

class EERegister;

namespace Ui {
class EELogin;
}

class EELogin : public EEWidget
{
    Q_OBJECT

public:
    explicit EELogin(EESDK *sdk, QWidget *parent = 0);
    ~EELogin();

private:
    bool isEmailValid();

private slots:
    void on_pushButtonLogin_clicked();

    void on_pushButtonRegister_clicked();

    void on_lineEditEmail_textChanged(const QString &arg1);

    void on_lineEditPassword_textChanged(const QString &arg1);

    void requestError(EEError, QString);
    void bucketsReceived(QList<EEBucket>);

private:
    Ui::EELogin *ui;
    EERegister *mRegister;
};

#endif // EELOGIN_H
