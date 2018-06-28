#include "eeregister.h"
#include "ui_eeregister.h"
#include "eesettingsclass.h"

#include <QCryptographicHash>

EERegister::EERegister(EESDK *sdk, QWidget *parent) :
    EEWidget(sdk, parent),
    ui(new Ui::EERegister)
{
    ui->setupUi(this);

    ui->lineEditEmail->setStyleSheet("border: 1px solid gray");
    ui->lineEditPassword->setStyleSheet("border: 1px solid gray");
    ui->lineEditPasswordRepeat->setStyleSheet("border: 1px solid gray");

    connect(sdk, SIGNAL(requestError(EEError,QString)), this, SLOT(requestError(EEError,QString)));
    connect(sdk, SIGNAL(userRegistered(EEUser)), this, SLOT(userRegistered(EEUser)));
}

EERegister::~EERegister()
{
    delete ui;
}

void EERegister::setCredentials(QString email, QString password)
{
    ui->lineEditEmail->setText(email);
    ui->lineEditPassword->setText(password);
}

bool EERegister::isPasswordValid()
{
    if (ui->lineEditPasswordRepeat->text() == ui->lineEditPassword->text()) {
        return true;
    } else {
        return false;
    }
}

bool EERegister::isEmailValid()
{
    QRegExp lMailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    lMailREX.setCaseSensitivity(Qt::CaseInsensitive);
    lMailREX.setPatternSyntax(QRegExp::RegExp);

    return lMailREX.exactMatch(ui->lineEditEmail->text());
}

void EERegister::warningMessage(QString message, EERegisterField field)
{
    switch (field) {
    case EERegisterFieldEmail:
        ui->lineEditEmail->setStyleSheet("border: 1px solid red");
        break;
    case EERegisterFieldPassword:
        ui->lineEditPassword->setStyleSheet("border: 1px solid red");
        break;
    case EERegisterFieldPasswordRepeat:
        ui->lineEditPasswordRepeat->setStyleSheet("border: 1px solid red");
        break;
    case EERegisterFieldNone:
        break;
    }

    ui->labelWarning->setText(message);
}
/**
 * @brief EERegister::on_pushButtonRegister_clicked
 * Try to registrate user
 */
void EERegister::on_pushButtonRegister_clicked()
{
    bool lProceed = true;

    warningMessage("", EERegisterFieldNone);

    if (ui->lineEditPasswordRepeat->text().isEmpty()) {
        warningMessage(tr("Please repeat the password"), EERegisterFieldPasswordRepeat);
        lProceed = false;
    }

    if (ui->lineEditPassword->text().isEmpty()) {
        warningMessage(tr("Password can't be blank"), EERegisterFieldPassword);
        lProceed = false;
    }

    if (ui->lineEditEmail->text().isEmpty()) {
        warningMessage(tr("Email can't be blank"), EERegisterFieldEmail);
        lProceed = false;
    }

    if (lProceed) {
        if (isPasswordValid() && isEmailValid()) {
            QString lPassword = QString(QCryptographicHash::hash((ui->lineEditPassword->text().toUtf8()), QCryptographicHash::Sha256).toHex());
            sdk()->registerUser(ui->lineEditEmail->text(), lPassword);
        } else if (!isPasswordValid()) {
            warningMessage(tr("Password doesn't match"), EERegisterFieldPasswordRepeat);
            warningMessage(tr("Password doesn't match"), EERegisterFieldPassword);
        } else {
            warningMessage(tr("Invalid email format"), EERegisterFieldEmail);
        }
    }
}

void EERegister::on_lineEditEmail_textChanged(const QString&)
{
    ui->lineEditEmail->setStyleSheet("border: 1px solid gray");
}

void EERegister::on_lineEditPassword_textChanged(const QString&)
{
    ui->lineEditPassword->setStyleSheet("border: 1px solid gray");
}

void EERegister::requestError(EEError err, QString method)
{
    if (method == EESDK::mMtdRegisterUser) {
        warningMessage(err.message(), EERegisterFieldNone);
    }
}
/**
 * @brief EERegister::userRegistered
 * After succesful registration - log user in
 */
void EERegister::userRegistered(EEUser)
{
    if (ui->checkBoxRemember->isChecked()) {
        EESettingsClass::getInstance().setSettingsValue(SettingsOptions::Email, ui->lineEditEmail->text());
        EESettingsClass::getInstance().setSettingsValue(SettingsOptions::Password, ui->lineEditEmail->text());
    }

    EESettingsClass::getInstance().setSettingsValue(SettingsOptions::Remember, static_cast<int>(ui->checkBoxRemember->isChecked()));

    close();
    emit userRegistredLoggedIn();
}
