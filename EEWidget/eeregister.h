#ifndef EEREGISTER_H
#define EEREGISTER_H

#include "eewidget.h"

#include <QWidget>

typedef enum {
    EERegisterFieldNone,
    EERegisterFieldEmail,
    EERegisterFieldPassword,
    EERegisterFieldPasswordRepeat
} EERegisterField;


namespace Ui {
class EERegister;
}

class EERegister : public EEWidget
{
    Q_OBJECT

public:
    explicit EERegister(EESDK *sdk, QWidget *parent = 0);
    ~EERegister();

    void setCredentials(QString email, QString password);

private:
    bool isPasswordValid();
    bool isEmailValid();
    void warningMessage(QString, EERegisterField);

private slots:
    void on_pushButtonRegister_clicked();

    void on_lineEditEmail_textChanged(const QString &arg1);

    void on_lineEditPassword_textChanged(const QString &arg1);

    void requestError(EEError, QString);

private:
    Ui::EERegister *ui;
};

#endif // EEREGISTER_H
