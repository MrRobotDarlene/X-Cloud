#ifndef EEBUCKETDOWNLOADING_H
#define EEBUCKETDOWNLOADING_H

#include "eebucketbasehandler.h"

#include <QQueue>
#include <QList>

class EEFile;
class EEBucket;

/**
 * @brief The EEBucketDownloadingHandler class
 * Class, which are in charge of buckets downloading.
 * Holds queue of buckets to download and download them one by one
 * Downloading bucket queue MUST be sorted all of the time
 * In other case comparation with local data and json won't work correct
 */
class EEBucketDownloadingHandler:  public EEBucketBaseHandler
{
public:
    EEBucketDownloadingHandler(EEBucketDataHolder *bucketData);
    ~EEBucketDownloadingHandler();

    void initializeDownloadingQueue();
    QQueue<EEBucket *> downloadingBucketsQueue() const;

    /**
     * @brief EEBucketDownloadingHandler::addBucketToDownloadingQueue
     * Downloading queue is always sorted
     * So, when we try to add new value we search correct place to insert it
     * Using inserting sorting algorithm
     * @param bucket
     */
    void addBucketToDownloadingQueue(EEBucket* bucket);
    bool setCurrentBucketByDownloadingQueue();

    /**
     * @brief EEBucketDownloadingHandler::setDownloadingBucketQueue
     * Set all of the buckets into downloading queue and sort them
     * @param buckets
     */
    void setDownloadingBucketQueue(QList<EEBucket*>buckets);
    bool removeBucketFromDownloadingQueue(EEBucket *bucket);

    void setTemporaryDownloadingQueueFiles(QList<EEFile *> files);
    bool removeFileFromTemporaryDownloadingQueue(EEFile* removeFile);
    int temporaryBucketFilesListSize();
    EEFile* temporaryBucketCurrentFile() const;

    void addFilesListToQueue();
private:
    /**
     * @brief EEBucketDownloadingHandler::sortBucketQueue
     * use selection sorting
     * sort in asceding order by folder name
     * last element will be root
     */
    void sortBucketQueue();
private:
    QQueue<EEBucket*> mDownloadingBucketQueue;
    QList<EEFile*> mTemporaryFileList;
};

#endif // EEBUCKETDOWNLOADING_H
