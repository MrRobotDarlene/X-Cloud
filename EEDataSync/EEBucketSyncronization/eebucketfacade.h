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
class EEBucketDeletionHandler;
class EEOutdatedDataHolder;
class EEBaseOutdatedDataHolder;


class EEFolderModel;
class EEModel;

/**
 * @brief The EEBucketWrapper class
 * Class - facade, which is manage work with all processes related
 * to download, upload, update or delete data from/to cloud
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
    EEFile *currentUploadingFile();

    void addModelToUploadFilesQueue(QString fileName, QString bucketId);
    QQueue<EEFile *> uploadingFilesQueue() const;


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

    void setCurrentFileName(QString file);
    QString currentFileName() const;

    void addFilesListByBucketId(QString bucket, QList<EEFile *> files);
    QList<EEFile *> filesByBucketId(QString bucketId);
    EEFile *bucketFileByName(QString fileName, QString bucketId);
    bool removeFileFromBucket(QString fileName, QString bucketId);


    bool setNextCurrentBucket();

    bool removeBucketByBucketId(QString bucketId);
    bool removeBucketFromTreeById(QString bucketId);

    void buildLocalFormFromBucket();
    EEFolderModel *bucketsFolderModelForm() const;

    //for downloading
    void initializeDownloadingQueue();
    QQueue<EEBucket *> downloadingBucketsQueue() const;
    bool setCurrentBucketByDownloadingQueue();
    void setDownloadingBucketQueue(QList<EEBucket*>buckets);

    bool removeBucketFromDownloadingQueue(EEBucket *bucket);

    void setTemporaryDownloadingQueueFiles(QList<EEFile *> files);
    bool removeFileFromTemporaryDownloadingQueue(EEFile *file);
    int temporaryBucketFilesListSize();
    EEFile *temporaryBucketCurrentFile() const;

    void addCurrentFilesToDownloadQueue();
    bool setCurrentFileByDownloadingQueue();
    EEFile *currentDownloadingFile();
    QQueue<EEFile *> downloadingFileQueue() const;


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
    void addFileToUpdateFileQueue(EEFile *file);

    QQueue<EEFile *> updatingFilesQueue();

    bool setCurrentFileByUpdatingQueue();
    EEFile *currentUpdatingFile();


    //for deletion
    void addToDeletionBucketQueue(EEBucket *bucket);
    EEBucket *nextBucketToRemove();
    void addToDeletionFilesList(EEFile *file);
    bool setCurrentFileByDeletionQueue();
    EEFile *currentDeletionFile();


    void initializeUploadingBucketsQueueFromUpdateQueue();
    bool setCurrentFolderModelByUpdatingBucket();
    void setCurrentBucketsQueueByUpdatingBucket();
    void setUpdateFilesQueueToUploadQueue();

    //outdate deletion
    void addOutdatedCloudFile(EEFile *file);
    void addOutdatedLocalFile(QString fileName);
    void addOutdateFolderToRemoveList(EEFolderModel *folderModel);

    QList<EEFolderModel *> outdatedLocalFolders() const;

    QList<EEFile *> outDatedCloudFiles() const;
    EEFile *firstOutDateElement() const;
    void removeFirstOutdated();

    QList<EEFolderModel *> outDatedLocalFolderWithFilesToRemove() const;
    void clearOutDatedFiles();

    void clearOutDatedFolderList();



    void addCorruptedCloudFile(EEFile *file);
    void addCorruptedLocalFile(QString fileName);

    QList<EEFile *> corruptedCloudFiles() const;
    EEFile *firstCorruptedElement() const;
    void removeFirstCorrupted();

    QList<EEFolderModel *> corruptedFoldersWithFilesToRemove() const;
    void clearCorruptedLocalValues();

private:
    EEBucketDataHolder *mBucketDataHolder;

    EEBucketDownloadingHandler *mDownloadingHandler;
    EEBucketUploadingHandler *mUploadingHandler;
    EEBucketUpdatingHandler *mUpdatingHandler;
    EEBucketDeletionHandler *mBucketDeletionHandler;
    EEOutdatedDataHolder *mOutdatedDataHolder;
    EEBaseOutdatedDataHolder *mCorruptedDataHolder;
};

#endif // EEREMOTEBUCKETCONTROLLER_H
