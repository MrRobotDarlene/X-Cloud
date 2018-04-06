#ifndef EEFOLDERMODEL_H
#define EEFOLDERMODEL_H

#include "eemodel.h"

#include <QObject>
#include <QDateTime>

/**
 * @brief The EEFolderModel class
 * Holds folder information:
 * last folder update(update of the last file in folder) - mUpdated
 * folder name - mName
 * list of subfolders - mSubdirs
 */
class EEFolderModel : public EEModel
{
    Q_OBJECT
public:
    explicit EEFolderModel(EEModel *parent = nullptr);

    ~EEFolderModel();

    /**
     * @brief EEFolderModel::cleanChildrenList
     * delete all subfolders and subfiles
     */
    void cleanChildrenList();
    /**
     * @brief isEmpty
     * check does it have children
     * @return
     */
    bool isEmpty();
    bool addFolder(EEFolderModel *folder);

    /**
     * @brief EEFolderModel::addFile
     * Add file to list of child files
     * If it's not exists yet
     * @param file
     * @return
     */
    bool addFile(EEModel *file);

    /**
     * @brief getChildrenList
     * @return status
     */
    QList<EEFolderModel *> folderList() const;

    /**
     * @brief deleteFile
     * @return
     */
    bool deleteFile(EEModel*);

    /**
     * @brief deleteFolder
     * @return status
     */
    bool deleteFolder(EEFolderModel* folder);

    /**
     * @brief filesList
     * @return
     */
    QList<EEModel *> filesList() const;

    inline bool operator ==(EEFolderModel &model) {
        return EEModel::operator ==(model);
    }

private:
    QList<EEFolderModel *> mSubdirs;
    QList<EEModel *> mFiles;
};

#endif // EEFOLDERMODEL_H
