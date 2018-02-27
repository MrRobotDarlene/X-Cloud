#ifndef EEJSONBUILDER_H
#define EEJSONBUILDER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class EEFolderModel;

class EEJsonBuilder : public QObject
{
    Q_OBJECT
public:
    explicit EEJsonBuilder(QObject *parent = nullptr);

    /**
     * @brief EEJsonBuilder::buildJsonDocument
     * Dynamically builds json document based on json object,
     * which holds information about folders
     * @param rootDirectory
     * @return - json document with info about folder
     */
    static QJsonDocument buildJsonDocument(EEFolderModel *rootDirectory);

private:
    /**
     * @brief EEJsonBuilder::buildJsonObject
     * Dynamically builds json object with information about folders/subfolders
     * @param directory
     * @return - json object with info about folders
     */
    static QJsonObject buildJsonObject(EEFolderModel *directory);
private:
};

#endif // EEJSONBUILDER_H
