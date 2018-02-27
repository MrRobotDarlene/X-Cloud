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
    bool addFolder(EEFolderModel *child);

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
    QList<EEFolderModel *> getFolderList() const;

    /**
     * @brief deleteFile
     * @return
     */
    bool delelteFile(EEModel*);

    /**
     * @brief deleteFolder
     * @return status
     */
    bool deleteFolder(EEFolderModel*);


    QList<EEModel *> getFilesList() const;

private:
    QList<EEFolderModel *> mSubdirs;
    QList<EEModel *> mFiles;
};

#endif // EEFOLDERMODEL_H
