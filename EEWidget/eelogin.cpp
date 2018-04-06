#include "eelogin.h"
#include "ui_eelogin.h"

#include "eeregister.h"
#include "eesettingsclass.h"

#include <QDebug>

#define STYLE_OK    "border: 1px solid gray"
#define STYLE_ERROR "border: 1px solid red"

EELogin::EELogin(EESDK *sdk, QWidget *parent) :
    EEWidget(sdk, parent),
    ui(new Ui::EELogin),
    mRegister{new EERegister(sdk, parent)}
{
    ui->setupUi(this);

    ui->lineEditEmail->setStyleSheet(STYLE_OK);
    ui->lineEditPassword->setStyleSheet(STYLE_OK);


    bool lIsChecked = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::Remember).toInt();
    if (lIsChecked) {
        ui->lineEditEmail->setText(EESettingsClass::getInstance().getSettingsValue(SettingsOptions::Email).toString());
        ui->lineEditPassword->setText(EESettingsClass::getInstance().getSettingsValue(SettingsOptions::Password).toString());
        ui->checkBoxRemember->setChecked(lIsChecked);
    }

    connect(sdk, SIGNAL(requestError(EEError,QString)), this, SLOT(requestError(EEError,QString)));
    connect(mRegister, SIGNAL(userRegistredLoggedIn()), this, SIGNAL(userLoggedIn()));
}

EELogin::~EELogin()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    delete mRegister;
    delete ui;
}

/**
 * @brief EELogin::on_pushButtonLogin_clicked
 * Try to log in.
 * If seems as succesful - ping server with buckets request
 */
void EELogin::on_pushButtonLogin_clicked()
{
    bool lProceed = true;
    if (ui->lineEditPassword->text().isEmpty()) {
        lProceed = false;
        ui->lineEditPassword->setStyleSheet(STYLE_ERROR);
    }

    if (!isEmailValid()) {
        lProceed = false;
        ui->lineEditEmail->setStyleSheet(STYLE_ERROR);
    }

    if (lProceed) {
        connect(sdk(), SIGNAL(bucketsReceived(QList<EEBucket*>)), this, SLOT(serverPinged(QList<EEBucket*>)));
        sdk()->setCredentials(ui->lineEditEmail->text(), ui->lineEditPassword->text());

        sdk()->getBuckets();
    }
}

/**
 * @brief EELogin::on_pushButtonRegister_clicked
 * Call registration window
 */
void EELogin::on_pushButtonRegister_clicked()
{
    hide();

    //mRegister->setCredentials(ui->lineEditEmail->text(), ui->lineEditPassword->text());
    mRegister->show();
}

bool EELogin::isEmailValid()
{
    QRegExp lMailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    lMailREX.setCaseSensitivity(Qt::CaseInsensitive);
    lMailREX.setPatternSyntax(QRegExp::RegExp);

    return lMailREX.exactMatch(ui->lineEditEmail->text());
}

void EELogin::on_lineEditEmail_textChanged(const QString &)
{
    ui->lineEditEmail->setStyleSheet(STYLE_OK);
}

void EELogin::on_lineEditPassword_textChanged(const QString &)
{
    ui->lineEditPassword->setStyleSheet(STYLE_OK);
}
/**
 * @brief EELogin::requestError
 * Calls in case of error responce from server
 * @param err
 * @param method
 */
void EELogin::requestError(EEError err, QString method)
{
    if (method == MTD_GET_BUCKETS) {
        ui->labelMessage->setText(err.message());
    }
}

/**
 * @brief EELogin::serverPinged
 * After logging in - ping server with request for buckets to check corectness of signing in
 * @param buckets
 */
void EELogin::serverPinged(QList<EEBucket*> buckets)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    disconnect(sdk(), SIGNAL(bucketsReceived(QList<EEBucket*>)), this, SLOT(serverPinged(QList<EEBucket*>)));

    if (buckets.isEmpty()) {
        qDeleteAll(buckets);
        buckets.clear();
    }

    qDebug() << "Login succeed!";

    if (ui->checkBoxRemember->isChecked()) {
        EESettingsClass::getInstance().setSettingsValue(SettingsOptions::Email, ui->lineEditEmail->text());
        EESettingsClass::getInstance().setSettingsValue(SettingsOptions::Password, ui->lineEditPassword->text());
    }

    EESettingsClass::getInstance().setSettingsValue(SettingsOptions::Remember, static_cast<int>(ui->checkBoxRemember->isChecked()));

    close();
    emit userLoggedIn();
}
