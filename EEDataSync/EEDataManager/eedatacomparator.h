#ifndef EEDATACOMPARATOR_H
#define EEDATACOMPARATOR_H

#include <QObject>
#include "EEDataSync/EELocalDataParser/eemodel.h"
#include "EEDataSync/EELocalDataParser/eefoldermodel.h"

class EESDK;
class EEBucketFacade;
class EEFolderParseController;
class EEFile;

/**
 * @brief The EEDataComparator class
 * Class, used to compare local data
 * with cloud data. Based on this comparation
 * build folders, files, which has to be uploaded, downloaded or deleted
 */
class EEDataComparator : public QObject
{
    Q_OBJECT
public:
    explicit EEDataComparator(EESDK *sdk, EEBucketFacade *facade, EEFolderParseController *folderParseController, QObject *parent = nullptr);

    /**
     * @brief EEDataComparator::initializeComparationLocalDataWithJson
     * Initialize process of comparation local data with json data
     * @param jsonFolderModel
     */
    void initializeOutdatedDataComparation(EEFolderModel *jsonFolderModel, EEFolderModel *cloudFolderModel);
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
     * @brief EEDataComparator::addFolderWithSubfoldersToDeletionList
     * Recursively add bucket and its subbuckets to deletion list
     * @param model
     */
    void addFolderWithSubfoldersToDeletionList(EEFolderModel *model);

    /**
     * @brief EEDataComparator::compareLocalFilesWithJson
     * If file is not exists locally anymore - add to deletion files list
     * @param localFolder - local folder, which files have to be compared
     * @param jsonFolder - same folder as it described in json
     */
    void compareLocalFilesWithJson(EEFolderModel *localFolder, EEFolderModel *jsonFolder, EEFolderModel *cloudFolder);

    /**
     * @brief EEDataComparator::compareLocalFoldersWithJson
     * Compare local folders with json.
     * If exists in both - start to compare files too
     * IF exists in json and not locally - add to deletion list
     * @param localFolder - root local folder
     * @param jsonFolder - root json folder
     */
    void compareLocalFoldersWithJson(EEFolderModel *localFolder, EEFolderModel *jsonFolder, EEFolderModel *cloudFolder);
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
    void buildUpdateFilesForFolder(QList<EEModel *> localFiles, QList<EEFile *> cloudFiles, QString bucketName);


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
    bool listHasName(QList<EEFile *> list, QString name);


signals:
    void dataCompared();
private:
    EESDK *mSdk;
    EEBucketFacade *mBucketFacade;
    EEFolderParseController *mFolderParseController;
};

#endif // EEDATACOMPARATOR_H
