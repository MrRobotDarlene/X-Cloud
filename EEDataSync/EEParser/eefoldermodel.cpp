#include "eefoldermodel.h"

#include <QList>
#include <QString>

EEFolderModel::EEFolderModel(EEModel *parent) : EEModel(parent)
{

}

EEFolderModel::~EEFolderModel()
{
    if (mSubdirs.size() > 0) {
        qDeleteAll(mSubdirs);
        mSubdirs.clear();
    }

    if (mFiles.size() > 0) {
        qDeleteAll(mFiles);
        mFiles.clear();
    }
}
/**
 * @brief EEFolderModel::cleanChildrenList
 * delete all subfolders and subfiles
 */
void EEFolderModel::cleanChildrenList()
{
    if (mSubdirs.size() > 0) {
        qDeleteAll(mSubdirs);
        mSubdirs.clear();
    }

    if (mFiles.size() > 0) {
        qDeleteAll(mFiles);
        mFiles.clear();
    }
}

bool EEFolderModel::isEmpty()
{
    bool lIsEmpty = false;
    if (!mSubdirs.count()) {
        if (!mFiles.count()) {
            lIsEmpty = true;
        }
    }

    return lIsEmpty;
}
/**
 * @brief EEFolderModel::addChild
 * Add child to list of children
 * If it's not exists yet
 * @param child
 * @return
 */
bool EEFolderModel::addFolder(EEFolderModel *folder)
{
    if (folder != nullptr) {
        foreach (EEFolderModel *tmpFolder, mSubdirs) {
            if (folder->name() == tmpFolder->name()) {
                return false;
            }
        }
        mSubdirs.append(folder);
        return true;
    } else {
        return false;
    }
}

bool EEFolderModel::addFile(EEModel *file)
{
    if (file != nullptr) {
        foreach (EEFolderModel *tmpFile, mSubdirs) {
            if (file->name() == tmpFile->name()) {
                return false;
            }
        }
        mFiles.append(file);
        return true;
    } else {
        return false;
    }
}
/**
 * @brief EEFolderModel::getChildrenList
 * @return
 */
QList<EEFolderModel *> EEFolderModel::folderList() const
{
    return mSubdirs;
}

bool EEFolderModel::deleteFile(EEModel *file)
{
    if (file != nullptr) {
        return mFiles.removeOne(file);
    }

    return false;
}

bool EEFolderModel::deleteFolder(EEFolderModel *folder)
{
    if (folder != nullptr) {
        return mSubdirs.removeOne(folder);
    }

    return false;
}

QList<EEModel *> EEFolderModel::filesList() const
{
    return mFiles;
}
