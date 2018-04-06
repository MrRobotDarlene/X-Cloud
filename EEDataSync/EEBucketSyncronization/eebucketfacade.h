#ifndef EEREMOTEBUCKETCONTROLLER_H
#define EEREMOTEBUCKETCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QString>

class EEFile;
class EEBucket;

class EEBucketDataHolder;
class EEBucketUploadingHandler;
class EEBucketDownloadingHandler;
class EEBucketUpdatingHandler;


class EEFolderModel;
class EEModel;

/**
 * @brief The EEBucketWrapper class
 * Class - facade, which is manage work with all processes related
 * to download/upload data from cloud
 */
class EEBucketFacade : public QObject
{
    Q_OBJECT
public:
    explicit EEBucketFacade(QObject *parent = nullptr);

    ~EEBucketFacade();
    //for uploading
    void addElementToUploadBucketQueue(EEFolderModel *folder);
    bool setCurrentFolderModelByUploadingBucket();

    QQueue<EEFolderModel *> uploadingBucketsQueue() const;
    EEFolderModel *currentUploadingFolderModel() const;
    QString currentUploadingFolderName() const;
    int currentUploadingFolderSize() const;
    QString currentUploadingFileNameFromFolderModel() const;

    bool setCurrentFileByUploadingQueue();
    EEFile currentUploadingFile();

    void addModelToUploadFilesQueue(QString fileName, QString bucketId);
    QQueue<EEFile> uploadingFilesQueue() const;


    //for both
    QList<EEBucket *> allBuckets() const;
    void setAllBuckets(QList<EEBucket *> bucketList);
    void addBucketToAllBuckets(EEBucket *newBucket);

    void setCurrentFileIndex(int index);
    int currentFileIndex() const;
    void increaseCurrentIndex();

    void setCurrentBucket(EEBucket *bucket);
    EEBucket *currentBucket() const;
    EEBucket *bucketById(QString bucketId) const;

    void addFilesListByBucketId(QString bucket, QList<EEFile> files);
    QList<EEFile> filesByBucketId(QString bucketId);

    bool setNextCurrentBucket();

    bool removeBucketByBucketId(QString bucketId);



    //for downloading
    void initializeDownloadingQueue();
    QQueue<EEBucket *> downloadingBucketsQueue() const;
    bool setCurrentBucketByDownloadingQueue();
    void setDownloadingBucketQueue(QList<EEBucket*>buckets);

    bool removeBucketFromDownloadingQueue(EEBucket *bucket);

    void setOneBucketDownloadingQueueFiles(QList<EEFile> files);
    bool removeFileFromOneBucketDownloadingQueue(EEFile file);
    int currentFilesListSize();
    EEFile currentFile() const;

    void addCurrentFilesToDownloadQueue();
    bool setCurrentFileByDownloadingQueue();
    EEFile currentDownloadingFile();
    QQueue<EEFile> downloadingFileQueue() const;


    void setWorkingDirectory(QString directory);
    QString workingDirectory() const;

    //updating of files
    int currentUpdatingFolderSize() const;
    QString currentUpdatingFolderName() const;
    EEFolderModel *currentUpdatingFolderModel() const;
    QString currentUpdatingFileNameFromFolderModel() const;

    void addBucketToDownloadingQueue(EEBucket *bucket);
    void addBucketToUploadingQueue(EEFolderModel *folder);

    EEBucket *bucketByName(QString bucketName) const;
    EEBucket *rootBucket() const;
    EEBucket *lastBucket() const;

    QQueue<EEFolderModel *> updateBucketQueue() const;
    void addElementToUpdateBucketQueue(EEFolderModel *folder);
    void addFileToUpdateFileQueue(EEFile file);

    QQueue<EEFile> updatingFilesQueue();

    bool setCurrentFileByUpdatingQueue();
    EEFile currentUpdatingFile();



    void initializeUploadingBucketsQueueFromUpdateQueue();
    bool setCurrentFolderModelByUpdatingBucket();
    bool setCurrentBucketsQueueByUpdatingBucket();
    bool setUpdateFilesQueueToUploadQueue();

private:
    EEBucketDataHolder *mBucketDataHolder;

    EEBucketDownloadingHandler *mDownloadingHandler;
    EEBucketUploadingHandler *mUploadingHandler;
    EEBucketUpdatingHandler *mUpdatingHandler;
};

#endif // EEREMOTEBUCKETCONTROLLER_H
