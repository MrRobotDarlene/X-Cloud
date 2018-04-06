#include "eesettingsclass.h"

#include <QSettings>
#include <QApplication>
#include <QDebug>

const QString EESettingsClass::email("email");
const QString EESettingsClass::password("password");
const QString EESettingsClass::remember("remember");
const QString EESettingsClass::folderName("folder");


EESettingsClass::EESettingsClass(QObject *parent) : QObject(parent)
{
    mPathToSettings = QApplication::applicationDirPath() + QString("//temp//settings_profile.ini");
    mSettings = new QSettings(mPathToSettings, QSettings::IniFormat);
}

EESettingsClass::~EESettingsClass()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    delete mSettings;
}

EESettingsClass &EESettingsClass::getInstance()
{
    static EESettingsClass lInstance;

    return lInstance;
}
/**
 * @brief EESettingsClass::setSettingsValue
 * Save some value to settings
 * @param type
 * @param value
 */
void EESettingsClass::setSettingsValue(SettingsOptions type, QVariant value)
{
    SettingsOptions lType = type;
    switch (lType) {
    case SettingsOptions::Email:
        mSettings->setValue(email, value);
        break;
    case SettingsOptions::Password:
        mSettings->setValue(password, value);
        break;
    case SettingsOptions::Remember:
        mSettings->setValue(remember, value);
        break;
    case SettingsOptions::FolderName:
        mSettings->setValue(folderName, value);
        break;
    default:
        qDebug() << "No such settings key!";
        break;
    }
}
/**
 * @brief EESettingsClass::getSettingsValue
 * Get some value from settings
 * @param type
 * @return
 */
QVariant EESettingsClass::getSettingsValue(SettingsOptions type)
{
    SettingsOptions lType = type;

    QVariant lVariantValue;
    switch (lType) {
    case SettingsOptions::Email:
        lVariantValue = mSettings->value(email, "");
        break;
    case SettingsOptions::Password:
        lVariantValue = mSettings->value(password, "");
        break;
    case SettingsOptions::Remember:
        lVariantValue = mSettings->value(remember, 0);
        break;
    case SettingsOptions::FolderName:
        lVariantValue = mSettings->value(folderName, "");
        break;
    default:
        qDebug() << "No such settings key!";
        break;
    }
    return lVariantValue;
}
