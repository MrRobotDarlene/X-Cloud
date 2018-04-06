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
/**
 * @brief The EERegister class
 * Form for user registration. In case of succesful registration - log user in
 */
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

signals:
    void userRegistredLoggedIn();
private slots:
    /**
     * @brief EERegister::on_pushButtonRegister_clicked
     * Try to registrate user
     */
    void on_pushButtonRegister_clicked();

    void on_lineEditEmail_textChanged(const QString &arg1);

    void on_lineEditPassword_textChanged(const QString &arg1);

    void requestError(EEError, QString);
    /**
     * @brief EERegister::userRegistered
     * After succesful registration - log user in
     */
    void userRegistered(EEUser);

private:
    Ui::EERegister *ui;
};

#endif // EEREGISTER_H
