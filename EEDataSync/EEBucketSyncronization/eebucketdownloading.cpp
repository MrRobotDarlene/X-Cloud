#include "eebucketdownloadinghandler.h"
#include "EEContainers/eebucket.h"
#include "EEContainers/eefile.h"

#include <QDebug>

EEBucketDownloadingHandler::EEBucketDownloadingHandler(EEBucketDataHolder *bucketData):
    mBucketData{bucketData}
{
}

EEBucketDownloadingHandler::~EEBucketDownloadingHandler()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (!mDownloadingBucketQueue.isEmpty()) {
        qDeleteAll(mDownloadingBucketQueue);
        mDownloadingBucketQueue.clear();
    }
}

void EEBucketDownloadingHandler::addBucketToDownloadingQueue(EEBucket *bucket)
{
    mDownloadingBucketQueue.append(bucket);
}

void EEBucketDownloadingHandler::initializeDownloadingQueue()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    //doesn't need to be deleted here, because all buckets will removed in EEBucketDataHolder class
    if (!mDownloadingBucketQueue.isEmpty()) {
        mDownloadingBucketQueue.clear();
    }
    mDownloadingBucketQueue.append(mBucketData->allBuckets());
}

QQueue<EEBucket*> EEBucketDownloadingHandler::downloadingBucketsQueue() const
{
    return mDownloadingBucketQueue;
}

bool EEBucketDownloadingHandler::setCurrentBucketByDownloadingQueue()
{
    bool lIsBucketSetted = false;
    qDebug () << "Is empty: " << mDownloadingBucketQueue.isEmpty() << " count: " << mDownloadingBucketQueue.count();
    if (!mDownloadingBucketQueue.isEmpty()) {
        EEBucket * lBucket = new EEBucket(mDownloadingBucketQueue.dequeue());
        mBucketData->setCurrentBucket(lBucket);
        lIsBucketSetted = true;
    }
    return lIsBucketSetted;
}

void EEBucketDownloadingHandler::setDownloadingBucketQueue(QList<EEBucket *> buckets)
{
    if (!mDownloadingBucketQueue.isEmpty()) {
//        qDeleteAll(mDownloadingBucketQueue);
        mDownloadingBucketQueue.clear();
    }
    mDownloadingBucketQueue.append(buckets);
}

bool EEBucketDownloadingHandler::removeBucketFromDownloadingQueue(EEBucket *bucket)
{
    return mDownloadingBucketQueue.removeOne(bucket);
}

void EEBucketDownloadingHandler::setOneBucketDownloadingQueueFiles(QList<EEFile> files)
{
    mOneBucketFileList = files;
}

bool EEBucketDownloadingHandler::removeFileFromOneBucketDownloadingQueue(EEFile removeFile)
{
    return mOneBucketFileList.removeOne(removeFile);
}

QQueue<EEFile> EEBucketDownloadingHandler::downloadingFileQueue() const
{
    return mDownloadingFileQueue;
}

int EEBucketDownloadingHandler::currentFilesListSize()
{
    return mOneBucketFileList.size();
}

EEFile EEBucketDownloadingHandler::currentFile() const
{
    return mOneBucketFileList.at(mBucketData->currentFileIndex());
}

void EEBucketDownloadingHandler::addCurrentFilesToDownloadQueue()
{
    mDownloadingFileQueue.append(mOneBucketFileList);
    mOneBucketFileList.clear();
}

bool EEBucketDownloadingHandler::setCurrentFileByDownloadingQueue()
{
    bool lIsBucketSetted = false;
    if (!mDownloadingFileQueue.isEmpty()) {
        mCurrentDownloadingFile = mDownloadingFileQueue.dequeue();
        lIsBucketSetted = true;
    }
    return lIsBucketSetted;
}

EEFile EEBucketDownloadingHandler::currentDownloadingFile()
{
    return mCurrentDownloadingFile;
}


