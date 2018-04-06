#ifndef EEDATAMANAGER_H
#define EEDATAMANAGER_H

#include "EEContainers/eefile.h"

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

enum class EECurrentBucketsState {
    FileUpdating,
    NewBucketsFileUploading,
    NewBucketFilesDownloading,
    FileDateSyncronization,
    FileStatusCheck,
    FileDeletion,
    ExistingBucketFilesUploading,
    ExistingBucketFilesDownloading
};


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
     * @brief EEDataManager::deleteBucketsFromCloud
     * Delete all data from cloud and start to upload local
     */
    void deleteBucketsFromCloud();
    /**
     * @brief EEDataManager::initializeDeletionProcess
     * Start process of data deletion from cloud
     * It makes to upload new local data there
     */
    void initializeDeletionProcess();
    /**
     * @brief EEDataManager::buildDataQueue
     * Build queue of buckets. Using in order for folder tree
     * Folder equal to  bucket
     * @param folder
     */
    void buildUploadingDataQueue(EEFolderModel * const folder);

    /**
     * @brief EEDataManager::createBucket
     * Create bucket according to current bucket in usage
     */
    void createBucket();

    /**
     * @brief EEDataManager::uploadFile
     * Upload file to currently using bucket
     */
    void uploadFile();
    /**
     * @brief EEDataManager::startUseNextBucket
     * Move to the next bucket. Reset file index and send request for bucket creation
     */
    void createNextBucket();

    /**
     * @brief EEDataManager::getFilesListForCurrentBucket
     * Create directory for current bucket, if needed
     * Send get request for this bucket files
     */
    void getFilesListForCurrentBucket();

    /**
     * @brief EEDataManager::useNextBucketForDownloading
     * Initialize process of files donwloading for setted bucket.
     * After all files for created buckets were donwloaded - start to download files for existing buckets
     */
    void useNextBucketForDownloading();

    /**
     * @brief EEDataManager::downloadFile
     * Download file into restore library
     */
    void downloadFile();

    /**
     * @brief EEDataManager::filesListReceived
     * This method starts to using after receiving list of file
     * for current bucket and decryption of their names.
     * Call another method to start process of files downloading
     * @param files - list of files of backet
     */
    void filesListReceived(QList<EEFile> files);


    /**
     * @brief EEDataManager::setCurrentState
     * Set current program state
     * @param state
     */
    void setCurrentState(EECurrentBucketsState state);


    /**
    * @brief EEDataManager::uploadNextFiles
    * Upload files for existing buckets.
    * After uploading all of the buckets - start process of download required
    */
    void uploadNextFiles();
public slots:
    /**
     * @brief EEDataManager::checkCloudDataStatus
     * if local data has even been initialized before - download all the data from cloud
     * if cloud data hasn't been initalized yet or been initialized just part of data - delete it and upload all of the local
     * In others cases - start to compare all of the folders and files
     */
    void checkDataStatus();

    /**
     * @brief EEDataManager::initDataUpdate
     * After all neccesary folder/files for donwloading/uploading were built
     * start processes required for each of them
     */
    void startDataUpdate();

private slots:
    /**
     * @brief EEDataManager::handleCloudTreeReceived
     * Calls after data about buckets' files were received
     */
    void handleCloudTreeReceived();
    /**
     * @brief EEDataManager::handleFilesDownloading
     * Calls after succesful file download
     */
    void handleFilesDownloading();

    /**
     * @brief EEDataManager::handleFilesUploaded
     * Calls after file upload
     */
    void handleFilesUploaded();
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
    void handleFilesInfoReceived(QList<EEFile> files);

private:
    EESDK *mSdk;
    EEFileLoader *mLoader;
    EEFolderParseController *mFolderParseController;
    EEBucketFacade *mBucketFacade;
    EEDataTreeBuilder *mDataTreeBuilder;
    EEDateTimeSyncronizator *mDateTimeSyncronizator;
    EEDataComparator *mDataComparator;
    EEDataLoader *mDataLoader;


    EECurrentBucketsState mCurrentState;
};

#endif // EEDATAMANAGER_H
