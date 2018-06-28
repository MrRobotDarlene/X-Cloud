#ifndef EEDATALOADER_H
#define EEDATALOADER_H

#include <QObject>

class EESDK;
class EEBucketFacade;
class EEFileLoader;
class EEBucket;
class EEFolderParseController;
class EEFile;
class EEJsonBuilder;

/**
 * @brief The EEDataLoader class
 * Class, which used to upload/donwload data
 * in different situation and under influence of different circumstances
 */
class EEDataLoader : public QObject
{
    Q_OBJECT
public:
    explicit EEDataLoader(EESDK *sdk,
                          EEJsonBuilder *builder,
                          EEBucketFacade *facade,
                          EEFolderParseController *folderParseController,
                          EEFileLoader *loader,
                          QObject *parent = nullptr);

    /**
     * @brief EEDataLoader::deleteNextBucket
     * Delete bucket according to deletion queue.
     * After buckets deletion start to delete files
     */
    void deleteNextBucket();

    /**
     * @brief EEDataLoader::deleteNextFile
     * Delete file from bucket according to files deletion queue
     * After deletion emit signal about whole data complited deletion
     */
    void deleteNextFile();

    /**
     * @brief EEDataLoader::startDataUpdate
     * Start process of uploding/downloading/updating data if needed
     */
    void startDataUpdate();

    /**
     * @brief EEDataLoader::uploadNextFiles
     * Upload files for existing buckets.
     * After uploading all of the buckets - start process of download required
     */
    void uploadNextFiles();

    /**
     * @brief EEDataLoader::useNextBucketForDownloading
     * Initialize process of files donwloading for setted bucket.
     * After all files for created buckets were donwloaded - start to download files for existing buckets
     */
    void useNextBucketForDownloading();

    /**
     * @brief EEDataLoader::cloudFilesDownloading
     * Download files for existing bucket.
     * After start process of date syncronizatin
     */
    void cloudFilesDownloading();

    /**
     * @brief EEDataLoader::filesListReceived
     * This method starts to using after receiving list of file
     * for current bucket and decryption of their names.
     * Call another method to start process of files downloading
     * @param files - list of files of backet
     */
    void downloadingFilesListReceived();

    /**
     * @brief EEDataLoader::startDataDownloading
     * Download files for current bucket
     * or if all files have already been downloaded
     * move to the next bucket
     */
    void startDataDownloading();

    /**
     * @brief EEDataLoader::bucketsListReceived
     * Slot, which activate after receiving list of buckets.
     * Initialize list of buckets and queue of buckets to download
     * Start process of downloading
     * @param buckets - list of buckets
     */
    void bucketsListReceived();

    /**
     * @brief EEDataLoader::startUseNextBucket
     * Move to the next bucket. Reset file index and send request for bucket creation.
     * After all buckets were created - start to
     */
    void createNextBucket();

    /**
     * @brief EEDataLoader::startDataUploading
     * Control uploading of the files into bucket
     * and moving to the next one, of needed
     */
    void uploadBucketData();

private:
    /**
     * @brief EEDataLoader::createBucket
     * Create bucket according to current bucket in usage
     */
    void createBucket();

    /**
     * @brief EEDataLoader::getFilesListForCurrentBucket
     * Create directory for current bucket, if needed
     * Send get request for this bucket files
     */
    void getFilesListForCurrentBucket();

    /**
     * @brief EEDataLoader::uploadFile
     * Upload file to currently using bucket
     */
    void uploadFile();
    /**
     * @brief EEDataLoader::downloadFile
     * Download file into restore library
     */
    void downloadFile();

signals:
    void startFilesUploading();
    void uploadingFinished();
    void startFilesDownloading();

    void folderFilesDateSyncronization();

    void dataDeleted();
public slots:
    /**
     * @brief EEDataLoader::bucketCreated
     * Receive signal about bucket creation
     * and start to upload its data(files)
     * @param newbucket
     */
    void bucketCreated(EEBucket *newbucket);
    /**
     * @brief EEDataLoader::updateNextBucket
     * Delete neccesary buckets and then upload them from local machine
     */
    void updateNextBucket();

    /**
     * @brief EEDataManager::cloudFilesUpdating
     * Delete outdated files from cloud.
     * After deletion - start to upload new on theirs places
     */
    void cloudFilesUpdating();

private:
    EESDK *mSdk;
    EEJsonBuilder *mJsonBuilder;
    EEBucketFacade *mBucketFacade;
    EEFolderParseController *mFolderParseController;
    EEFileLoader *mLoader;
};

#endif // EEDATALOADER_H
