#ifndef EEBUCKETDOWNLOADING_H
#define EEBUCKETDOWNLOADING_H

#include "eebucketdataholder.h"
#include "EEContainers/eefile.h"

#include <QQueue>
#include <QList>

class EEFile;
class EEBucket;

/**
 * @brief The EEBucketDownloadingHandler class
 * Class, which are in charge of buckets downloading.
 * Holds queue of buckets to download and download them one by one
 */
class EEBucketDownloadingHandler
{
public:
    EEBucketDownloadingHandler(EEBucketDataHolder *bucketData);
    ~EEBucketDownloadingHandler();

    void initializeDownloadingQueue();
    QQueue<EEBucket *> downloadingBucketsQueue() const;

    void addBucketToDownloadingQueue(EEBucket* bucket);
    bool setCurrentBucketByDownloadingQueue();

    void setDownloadingBucketQueue(QList<EEBucket*>buckets);
    bool removeBucketFromDownloadingQueue(EEBucket *bucket);

    void setOneBucketDownloadingQueueFiles(QList<EEFile> files);
    bool removeFileFromOneBucketDownloadingQueue(EEFile removeFile);
    QQueue<EEFile> downloadingFileQueue() const;
    int currentFilesListSize();
    EEFile currentFile() const;


    void addCurrentFilesToDownloadQueue();
    bool setCurrentFileByDownloadingQueue();
    EEFile currentDownloadingFile();
private:
    EEBucketDataHolder *mBucketData;
    QQueue<EEBucket*> mDownloadingBucketQueue;
    QList<EEFile> mOneBucketFileList;
    QQueue<EEFile> mDownloadingFileQueue;
    EEFile mCurrentDownloadingFile;
};

#endif // EEBUCKETDOWNLOADING_H
