#ifndef EEDATAMANAGER_H
#define EEDATAMANAGER_H


#include <QObject>
#include <QQueue>

class EESDK;
class EEFolderModel;
class EEModel;
class EEFileLoader;
class EEFolderParseController;
class EEBucketFacade;
class EEBucket;
class EEDataTreeBuilder;
class EEDateTimeSyncronizator;
class EEDataComparator;
class EEDataLoader;
class EEDataRemover;
class EEFile;
class EEJsonBuilder;
class EEJsonValidator;
class EEJsonReader;

enum class EECurrentBucketsState {
    FileUpdating,
    NewBucketsFileUploading,
    NewBucketFilesDownloading,
    FileStatusCheck,
    CloudBucketsClear,
    ExistingBucketFilesUploading,
    ExistingBucketFilesDownloading,
    BucketFilesDateSyncronization,
    CorruptedDataDeletion,
    OutdatedDataDeletion,
    UnknownState
};


/**
 * @brief The EEDataManager class
 * Class, which manages all others classes
 * with calling needness method from them in different cases
 */
class EEDataManager : public QObject
{
    Q_OBJECT
public:
    explicit EEDataManager(EESDK *sdk, EEFileLoader *loader, QString path,  QObject *parent = nullptr);
    ~EEDataManager();

    /**
     * @brief EEDataManager::start
     * Start timer first time
     */
    void start();

    /**
     * @brief EEDataManager::initTimerForDataCheck
     * Initialize timer, which will check status each 1 minute
     * Also, check buckets in the beggind of the buckets run
     */
    void restartTimerForDataCheck();
    /**
    * @brief EEDataManager::stopTimerForDataCheck
    * Stops timer, which was checking status each 1 minute
    */
    void stopTimerForDataCheck();
    /**
     * @brief isTimerRunning
     * Check is timer running right now
     * @return
     */
    bool isTimerRunning() const;
    /**
     * @brief EEDataManager::initStorage
     * Build bucket queue from root folder
     * then start to upload them to the cloud
     * @param folder
     */
    void initStorage();
    /**
     * @brief EEDataManager::initLocalFromCloud
     * Initialize process of data donwloading to local pc
     * @param folder
     */
    void initLocalFromCloud();

private:
    /**
     * @brief EEDataManager::removeCorruptedValueAccordingToJson
     * Control process of corrupted files buiding and begin process of their removing
     */
    void removeCorruptedValueAccordingToJson();

    /**
     * @brief EEDataManager::deleteBucketsFromCloud
     * Delete all data from cloud and start to upload local
     */
    void deleteBucketsFromCloud();
    /**
     * @brief EEDataManager::initializeDeletionProcess
     * Start process of data deletion from cloud
     * It makes to upload new local data there
     */
    void initializeOutdatedFilesClear();
    /**
     * @brief EEDataManager::buildDataQueue
     * Build queue of buckets. Using in order for folder tree
     * Folder equal to  bucket
     * @param folder
     */
    void buildUploadingDataQueue(EEFolderModel * const folder);
    /**
     * @brief EEDataManager::setCurrentState
     * Set current program state
     * @param state
     */
    void setCurrentState(EECurrentBucketsState state);

public slots:
    /**
     * @brief EEDataManager::checkCloudDataStatus
     * Check is json created
     * If no - go directly to check data actuality
     * if yes - check for corrupted data and start to remove it from cloud and json
     */
    void checkCorruptionDataStatus();
    /**
     * @brief EEDataManager::checkDeletionDataStatus
     * Check is some of data is outdated
     * Remove folder/file, if needed
     */
    void checkDeletionDataStatus();
    /**
     * @brief EEDataManager::checkDataActuality
     *  * if local data has even been initialized before - download all the data from cloud
     * if cloud data hasn't been initalized yet or been initialized just part of data - delete it and upload all of the local
     * In others cases - start to compare all of the folders and files
     */
    void checkDataActuality();

    /**
     * @brief EEDataManager::initDataUpdate
     * After all neccesary folder/files for donwloading/uploading were built
     * start processes required for each of them
     */
    void startDataUpdate();

private slots:
    /**
     * @brief EEDataManager::handleBucketDeletion
     * deletion possibly can be called in 3 cases:
     *  files updating, removing outdated buckets or removing all of the files from cloud(if root doesn't exist)
     */
    void handleBucketDeletion();

    /**
     * @brief EEDataManager::handleFileDeletion
     * If updating process is on - keep removing needed files from updating
     * If corrupted data deletion - update status in json and begin to remove next file
     * if outdated data deletion - the same as with corrupted, but local files
     */
    void handleFileDeletion();
    /**
     * @brief EEDataManager::handleBucketsListReceive
     * After receiving of the buckets list,
     * it could possibly make different actions depends on current state
     * @param buckets
     */
    void handleBucketsListReceive(QList<EEBucket *> buckets);
    /**
     * @brief EEDataManager::handleFilesInfoReceived
     * Slot, which activates after receiving list of file for current bucket
     * then decrypt list of files names
     * using .c decryption
     * @param files - list of files
     */
    void handleFilesInfoReceived(QList<EEFile *> files);
    /**
     * @brief handleContinueNextProcess
     * After date time for file has been syncronizated - call previous process
     */
    void handleContinueNextProcess();

private:
    EESDK *mSdk;
    EEFileLoader *mLoader;
    EEFolderParseController *mFolderParseController;
    EEBucketFacade *mBucketFacade;

    EEJsonBuilder *mJsonBuilder;
    EEJsonValidator *mJsonValidator;
    EEJsonReader *mJsonReader;

    EEDataTreeBuilder *mDataTreeBuilder;
    EEDateTimeSyncronizator *mDateTimeSyncronizator;
    EEDataComparator *mDataComparator;
    EEDataLoader *mDataLoader;
    EEDataRemover *mDataRemover;

    EECurrentBucketsState mCurrentState;
};

#endif // EEDATAMANAGER_H
