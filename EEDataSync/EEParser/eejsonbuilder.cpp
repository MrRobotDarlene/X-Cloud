#include "eejsonbuilder.h"
#include "eefoldermodel.h"
#include "eemodel.h"

#include <QDebug>

#define KEY_SUBDIRS "Subdirs"
#define KEY_FILES   "Files"
#define KEY_NAME    "Name"
#define KEY_UPDATED "Updated"

EEJsonBuilder::EEJsonBuilder(QObject *parent)
    : QObject(parent)
{

}
/**
 * @brief EEJsonBuilder::buildJsonDocument
 * Dynamically builds json document based on json object,
 * which holds information about folders
 * @param rootDirectory
 * @return - json document with info about folder
 */
QJsonDocument EEJsonBuilder::buildJsonDocument(EEFolderModel *rootDirectory)
{
    QJsonObject lDebugObject(buildJsonObject(rootDirectory));
    QJsonDocument lJsonDocument(buildJsonObject(rootDirectory));

    return lJsonDocument;
}
/**
 * @brief EEJsonBuilder::buildJsonObject
 * Dynamically builds json object with information about folders/subfolders
 * @param directory
 * @return - json object with info about folders
 */
QJsonObject EEJsonBuilder::buildJsonObject(EEFolderModel *directory)
{
    QJsonObject lFolderObject;
    lFolderObject.insert(KEY_NAME, directory->name());
    lFolderObject.insert(KEY_UPDATED, directory->updated().toString());

    QJsonArray lSubdirs;
    foreach (EEFolderModel *dir, directory->folderList()) {
        lSubdirs.push_back(buildJsonObject(dir));
    }

    QJsonArray lFiles;
    QJsonObject lFileObject;
    foreach (EEModel *file, directory->filesList()) {
        lFileObject.insert(KEY_NAME, file->name());
        lFileObject.insert(KEY_UPDATED, file->updated().toString());
        lFiles.append(lFileObject);
    }


    lFolderObject.insert(KEY_FILES, lFiles);
    lFolderObject.insert(KEY_SUBDIRS, lSubdirs);

    return lFolderObject;
}
