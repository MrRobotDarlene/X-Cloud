#ifndef EESETTINGSCLASS_H
#define EESETTINGSCLASS_H

#include <QObject>
class QSettings;

enum class SettingsOptions {
    Email,
    Password,
    Remember,
    FolderName
};

/**
 * @brief The EESettingsClass class
 * Class, which is in charge of settings savings
 */
class EESettingsClass : public QObject
{
    Q_OBJECT
public:
    static EESettingsClass &getInstance();

    EESettingsClass(EESettingsClass&) = delete;
    EESettingsClass(EESettingsClass &&) = delete;
    EESettingsClass& operator=(const EESettingsClass&) = delete;
    EESettingsClass& operator=(const EESettingsClass&&) = delete;

    /**
     * @brief EESettingsClass::setSettingsValue
     * Save some value to settings
     * @param type
     * @param value
     */
    void setSettingsValue(SettingsOptions type, QVariant value);
    /**
     * @brief EESettingsClass::getSettingsValue
     * Get some value from settings
     * @param type
     * @return
     */
    QVariant getSettingsValue(SettingsOptions type);


    static const QString email;
    static const QString password;
    static const QString remember;
    static const QString folderName;

private:
    explicit EESettingsClass(QObject *parent = nullptr);
    ~EESettingsClass();

private:
    QSettings *mSettings;
    QString mPathToSettings;

    //constexpr auto mConstSettinsFilePath = "//temp//settings_profile.ini";
};

#endif // EESETTINGSCLASS_H
