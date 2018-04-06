#ifndef EEDATACOMPARATOR_H
#define EEDATACOMPARATOR_H

#include <QObject>
#include "EEDataSync/EEParser/eemodel.h"
#include "EEContainers/eefile.h"
#include "eefoldermodel.h"

class EESDK;
class EEBucketFacade;
class EEFolderParseController;

/**
 * @brief The EEDataComparator class
 * Class, used to compare local data
 * with cloud data. Based on this comparation
 * build folders, files, which has to be uploaded or downloaded
 */
class EEDataComparator : public QObject
{
    Q_OBJECT
public:
    explicit EEDataComparator(EESDK *sdk, EEBucketFacade *facade, EEFolderParseController *folderParseController, QObject *parent = nullptr);

    /**
     * @brief EEDataComparator::startCompareData
     * Start process of data comparation.
     * First, set all of the buckets to downloading buckets queue.
     * Later, if this bucket(folder) exists locally, it will be removed from the queue
     * Also, build files for updating list for root folder
     * and call recurcively buiding of list for other folders and theirs files
     */
    void startCompareData();
private:
    /**
     * @brief EEDataManager::buildUpdateDataQueue
     * Build list of buckets, which have to be updated
     * If bucket doesn't exist in cloud buckets list - add it
     * If exists - remove from downloading queue and start to compare all of the files
     * @param parentFolder
     */
    void buildUpdateFolders(EEFolderModel * const parentFolder);

    /**
     * @brief EEDataComparator::buildUpdateFilesForFolder
     * Check files list of local and cloud buckets.
     * Compare them and add to the downloading or uploading list
     * depends on result of comparation.
     * First, all of the files added to temporary downloading list
     * If one of them doens't need to be downloaded - remove from this list
     * and start to check does it need to be updated. If has to be updated, add to update files list.
     * After cycles are finished, add left files from temporary down. list to regular
     * @param localFiles - list of local files
     * @param cloudFiles - list of cloud files
     * @param bucketName
     */
    void buildUpdateFilesForFolder(QList<EEModel *> localFiles, QList<EEFile> cloudFiles, QString bucketName);


    /**
     * @brief listHasName
     * Compare cloud file with all files in the folder on the local machine
     * @param list - list of local files
     * @param name - cloud file name
     * @return
     */
    bool listHasName(QList<EEModel *> list, QString name);

    /**
     * @brief EEDataComparator::listHasName
     * Compare local file with all files in the folder(bucket) on the cloud
     * @param list - list of cloud files
     * @param name- local file name
     * @return
     */
    bool listHasName(QList<EEFile> list, QString name);


signals:

    void dataCompared();
public slots:
private:
    EESDK *mSdk;
    EEBucketFacade *mBucketFacade;
    EEFolderParseController *mFolderParseController;


};

#endif // EEDATACOMPARATOR_H
