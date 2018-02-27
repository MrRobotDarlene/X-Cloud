#include "eelogin.h"
#include "ui_eelogin.h"

#include "eeregister.h"

#include <QDebug>
#include <QSettings>

#define SETTINGS_EMAIL      "email"
#define SETTINGS_PASSWORD   "password"
#define SETTINGS_REMEMBER   "remember"

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

    QSettings lS;
    ui->lineEditEmail->setText(lS.value(SETTINGS_EMAIL, "").toString());
    ui->lineEditPassword->setText(lS.value(SETTINGS_PASSWORD, "").toString());
    ui->checkBoxRemember->setChecked(lS.value(SETTINGS_REMEMBER, false).toBool());

    connect(sdk, SIGNAL(requestError(EEError,QString)), this, SLOT(requestError(EEError,QString)));
    connect(sdk, SIGNAL(bucketsReceived(QList<EEBucket>)), this, SLOT(bucketsReceived(QList<EEBucket>)));
}

EELogin::~EELogin()
{
    delete mRegister;
    delete ui;
}

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
        sdk()->setCredentials(ui->lineEditEmail->text(), ui->lineEditPassword->text());

        sdk()->getBuckets();
    }
}

void EELogin::on_pushButtonRegister_clicked()
{
    hide();

    mRegister->setCredentials(ui->lineEditEmail->text(), ui->lineEditPassword->text());
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

void EELogin::requestError(EEError err, QString method)
{
    if (method == MTD_GET_BUCKETS) {
        ui->labelMessage->setText(err.message());
    }
}

void EELogin::bucketsReceived(QList<EEBucket> buckets)
{
    qDebug() << "Login succeed!";
    QSettings lS;

    if (ui->checkBoxRemember->isChecked()) {
        lS.setValue(SETTINGS_EMAIL, ui->lineEditEmail->text());
        lS.setValue(SETTINGS_PASSWORD, ui->lineEditPassword->text());
    }

    lS.setValue(SETTINGS_REMEMBER, ui->checkBoxRemember->isChecked());
}
