#include "eesyncronizationfolderform.h"
#include "ui_eesyncronizationfolderform.h"
#include "eesettingsclass.h"

#include <QFileDialog>
#include <QDebug>
#include <QStandardPaths>


EESyncronizationFolderForm::EESyncronizationFolderForm(EESDK *sdk, QWidget *parent) :
    EEWidget(sdk, parent),
    ui(new Ui::EESyncronizationFolderForm)
{
    ui->setupUi(this);
    this->setWindowTitle("Folder syncronization...");

    QString lFolderName = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::FolderName).toString();

    if (lFolderName.isEmpty()) {
        resetDefaultPath();
    }

    ui->folderName->setText(lFolderName);

    ui->folderName->setPlaceholderText(QString("Folder name"));
    ui->folderName->setReadOnly(true);
}

EESyncronizationFolderForm::~EESyncronizationFolderForm()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    delete ui;
}

void EESyncronizationFolderForm::resetDefaultPath()
{
    QString lXCloudPath = "/X Cloud";
#ifdef WIN32
    XCloudPath = "\\X Cloud";
#endif
    QString lPath =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + lXCloudPath;
    EESettingsClass::getInstance().setSettingsValue(SettingsOptions::FolderName, lPath);
}

void EESyncronizationFolderForm::on_buttonChooseFolder_clicked()
{
    QString lHomePath =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString lPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 lHomePath, QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    ui->folderName->setText(lPath);
}

void EESyncronizationFolderForm::on_buttonAccept_clicked()
{
    if (!ui->folderName->text().isEmpty()) {
        qDebug() << "Accept sync...";
        EESettingsClass::getInstance().setSettingsValue(SettingsOptions::FolderName, ui->folderName->text());

        close();
        emit newFolderChoosed(ui->folderName->text());
    } else {
        ui->folderName->setStyleSheet("border: 1px solid red");
    }
}

void EESyncronizationFolderForm::on_buttonCancel_clicked()
{
    close();
}
