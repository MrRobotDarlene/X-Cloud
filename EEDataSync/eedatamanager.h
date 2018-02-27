#ifndef EEDATAMANAGER_H
#define EEDATAMANAGER_H

#include <QObject>
#include <QQueue>
#include "EEContainers/eebucket.h"
#include "EEContainers/eefile.h"


class EESDK;
class EEFolderModel;
class EEModel;
class EEFileLoader;
class EEFolderParseController;
class EEBucketWrapper;

class EEDataManager : public QObject
{
    Q_OBJECT
public:
    explicit EEDataManager(EESDK *sdk, EEFileLoader *loader, QObject *parent = nullptr);
    ~EEDataManager();

    /**
     * @brief EEDataManager::initStorage
     * Build bucket queue from root folder
     * and sychs with cloud
     */

    /**
     * @brief EEDataManager::initStorage
     * Build bucket queue from root folder
     * then start to upload them to the cloud
     * @param folder
     */
    void initStorage(QString folder);


    void initLocalFromCloud(QString folder);

private:
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
    void startUploadNextBucket();

    /**
     * @brief EEDataManager::getFilesListForCurrentBucket
     * Create directory for current bucket, if needed
     * Send get request for this bucket files
     */
    void getFilesListForCurrentBucket();

    /**
     * @brief EEDataManager::startUseNextBucketForDownloading
     * Take first element of queue and set it as currently on usage
     */
    void startUseNextBucketForDownloading();

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

signals:
    void startToDownload();
public slots:
    /**
     * @brief EEDataManager::startDataDownloading
     * Download files for current bucket
     * or if all files have already been downloaded
     * move to the next bucket
     */
    void startDataDownloading();

    /**
     * @brief EEDataManager::startDataUploading
     * Control uploading of the files into bucket
     * and moving to the next one, of needed
     */
    void startDataUploading();

    /**
     * @brief EEDataManager::bucketCreated
     * Receive signal about bucket creation
     * and start to upload its data(files)
     * @param newbucket
     */
    void bucketCreated(EEBucket newbucket);

    /**
     * @brief EEDataManager::bucketsListReceived
     * Slot, which activate after receiving list of buckets.
     * Initialize list of buckets and queue of buckets to download
     * Start process of downloading
     * @param buckets - list of buckets
     */
    void bucketsListReceived(QList<EEBucket> buckets);

    /**
     * @brief EEDataManager::startFilesNamesDecryption
     * Slot, which activates after receiving list of file for current bucket
     * then decrypt list of files names
     * using .c decryption
     * @param files - list of files
     */
    void startFilesNamesDecryption(QList<EEFile> files);

private:
    EESDK *mSdk;
    EEFileLoader *mLoader;
    EEFolderParseController *mFolderParseController;
    EEBucketWrapper *mBucketWrapper;
};

#endif // EEDATAMANAGER_H
