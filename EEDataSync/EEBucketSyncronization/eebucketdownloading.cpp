#include "eebucketdownloadinghandler.h"
#include "EESDK/EEContainers/eebucket.h"
#include "EESDK/EEContainers/eefile.h"
#include "eebucketdataholder.h"

EEBucketDownloadingHandler::EEBucketDownloadingHandler(EEBucketDataHolder *bucketData):
    EEBucketBaseHandler{bucketData}
{
}

EEBucketDownloadingHandler::~EEBucketDownloadingHandler()
{
    if (!mDownloadingBucketQueue.isEmpty()) {
        //qDeleteAll(mDownloadingBucketQueue);
        mDownloadingBucketQueue.clear();
    }
}
/**
 * @brief EEBucketDownloadingHandler::addBucketToDownloadingQueue
 * Downloading queue is always sorted
 * So, when we try to add new value we search correct place to insert it
 * Using inserting sorting algorithm
 * @param bucket
 */
void EEBucketDownloadingHandler::addBucketToDownloadingQueue(EEBucket *bucket)
{
    bool lIsInserted = false;
    for (size_t i(0); i < mDownloadingBucketQueue.size(); ++i) {
        if (bucket->name().length() >= mDownloadingBucketQueue[i - 1]->name().length()) {
            if (bucket->name().length() <= mDownloadingBucketQueue[i]->name().length()) {
                mDownloadingBucketQueue.insert(i, bucket);
                lIsInserted = true;
            }
        }
    }

    if (!lIsInserted) {
        mDownloadingBucketQueue.append(bucket);
    }

}

void EEBucketDownloadingHandler::initializeDownloadingQueue()
{
    //doesn't need to be deleted here,
    //because all buckets will removed in EEBucketDataHolder class
    if (!mDownloadingBucketQueue.isEmpty()) {
        mDownloadingBucketQueue.clear();
    }
    mDownloadingBucketQueue.append(bucketData()->allBuckets());
    //downloading queue always has to be sorted!
    sortBucketQueue();
}

QQueue<EEBucket*> EEBucketDownloadingHandler::downloadingBucketsQueue() const
{
    return mDownloadingBucketQueue;
}

bool EEBucketDownloadingHandler::setCurrentBucketByDownloadingQueue()
{
    bool lIsBucketSetted = false;
    if (!mDownloadingBucketQueue.isEmpty()) {
        EEBucket * lBucket = new EEBucket(mDownloadingBucketQueue.dequeue());
        bucketData()->setCurrentBucket(lBucket);
        lIsBucketSetted = true;
    }
    return lIsBucketSetted;
}
/**
 * @brief EEBucketDownloadingHandler::setDownloadingBucketQueue
 * Set all of the buckets into downloading queue and sort them
 * @param buckets
 */
void EEBucketDownloadingHandler::setDownloadingBucketQueue(QList<EEBucket *> buckets)
{
    //deletion shoudn't be here!!!!
    if (!mDownloadingBucketQueue.isEmpty()) {
        mDownloadingBucketQueue.clear();
    }
    mDownloadingBucketQueue.append(buckets);
    //downloading queue always has to be sorted!
    sortBucketQueue();
}

bool EEBucketDownloadingHandler::removeBucketFromDownloadingQueue(EEBucket *bucket)
{
    return mDownloadingBucketQueue.removeOne(bucket);
}

void EEBucketDownloadingHandler::setTemporaryDownloadingQueueFiles(QList<EEFile*> files)
{
    mTemporaryFileList = files;
}

bool EEBucketDownloadingHandler::removeFileFromTemporaryDownloadingQueue(EEFile *removeFile)
{
    return mTemporaryFileList.removeOne(removeFile);
}

int EEBucketDownloadingHandler::temporaryBucketFilesListSize()
{
    return mTemporaryFileList.size();
}

EEFile *EEBucketDownloadingHandler::temporaryBucketCurrentFile() const
{
    return mTemporaryFileList.at(bucketData()->currentFileIndex());
}

void EEBucketDownloadingHandler::addFilesListToQueue()
{
    EEBucketBaseHandler::addFilesListToQueue(mTemporaryFileList);
    mTemporaryFileList.clear();
}
/**
 * @brief EEBucketDownloadingHandler::sortBucketQueue
 * use selection sorting
 * sort in asceding order by folder name
 * last element will be root
 */
void EEBucketDownloadingHandler::sortBucketQueue()
{
    for (size_t i(0); i < mDownloadingBucketQueue.size(); ++i) {
        size_t lCurrentMinimum = mDownloadingBucketQueue[i]->name().length();
        size_t lCurrentIndex = i;

        for (size_t j = i; j < mDownloadingBucketQueue.size(); ++j) {
            if (mDownloadingBucketQueue[j]->name().length() < lCurrentMinimum) {
                lCurrentMinimum = mDownloadingBucketQueue[j]->name().length();
                lCurrentIndex = j;
            }
        }

        if (lCurrentIndex != i) {
            mDownloadingBucketQueue.swap(i, lCurrentIndex);
        }
    }
}


