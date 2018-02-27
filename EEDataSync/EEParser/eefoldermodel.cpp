#include "eefoldermodel.h"

#include <QList>
#include <QString>

EEFolderModel::EEFolderModel(EEModel *parent) : EEModel(parent)
{

}

EEFolderModel::~EEFolderModel()
{
    if (mSubdirs.size() > 0) {
        mSubdirs.clear();
    }

    if (mFiles.size() > 0) {
        mFiles.clear();
    }
}
/**
 * @brief EEFolderModel::addChild
 * Add child to list of children
 * If it's not exists yet
 * @param child
 * @return
 */
bool EEFolderModel::addFolder(EEFolderModel *child)
{
    if (child != nullptr) {
        foreach (EEFolderModel *tmpChild, mSubdirs) {
            if (child == tmpChild) {
                return false;
            }
        }
        mSubdirs.append(child);
        return true;
    } else {
        return false;
    }
}

bool EEFolderModel::addFile(EEModel *file)
{
    if (file != nullptr) {
        foreach (EEFolderModel *tmpFile, mSubdirs) {
            if (file == tmpFile) {
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
QList<EEFolderModel *> EEFolderModel::getFolderList() const
{
    return mSubdirs;
}

bool EEFolderModel::delelteFile(EEModel *file)
{
    if (file != nullptr) {
        return mFiles.removeOne(file);
    }

    return false;
}

bool EEFolderModel::deleteFolder(EEFolderModel *child)
{
    if (child != nullptr) {
        return mSubdirs.removeOne(child);
    }

    return false;
}

QList<EEModel *> EEFolderModel::getFilesList() const
{
    return mFiles;
}
