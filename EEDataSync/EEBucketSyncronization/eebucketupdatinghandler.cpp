#include "eebucketupdatinghandler.h"
#include <QDebug>

EEBucketUpdatingHandler::EEBucketUpdatingHandler(EEBucketDataHolder *bucketData):
    EEBucketUploadingHandler{bucketData}
{
}

void EEBucketUpdatingHandler::addElementToUpdateBucketQueue(EEFolderModel *folder)
{
    addElementToUploadingBucketQueue(folder);
}

QQueue<EEFile> EEBucketUpdatingHandler::updatingFilesQueue() const
{
    return uploadingFilesQueue();
}

void EEBucketUpdatingHandler::addFileToUpdateFileQueue(EEFile newFile)
{
    addFileToUploaingFileQueue(newFile);
}

bool EEBucketUpdatingHandler::setCurrentFileByUpdatingQueue()
{
    return setCurrentFileByUploadingQueue();
}

EEFile EEBucketUpdatingHandler::currentUpdatingFile()
{
    return currentUploadingFile();
}

bool EEBucketUpdatingHandler::setCurrentFolderModelByUpdatingBucket()
{
    return setCurrentFolderModelByUploadingBucket();
}

QQueue<EEFolderModel *> EEBucketUpdatingHandler::updatingBucketQueue() const
{
    return uploadingBucketsQueue();
}

EEBucket* EEBucketUpdatingHandler::rootBucket() const
{
    EEBucket *lRoot = nullptr;
    auto lBuckets = mBucketData->allBuckets();
    for (auto bucket : lBuckets) {
        if (bucket->name() == '/') {
            lRoot = bucket;
            break;
        }
    }
    return lRoot;
}

EEBucket* EEBucketUpdatingHandler::lastBucket() const
{
    auto lBuckets = mBucketData->allBuckets();
    EEBucket * lLastBucket = nullptr;

    if (!lBuckets.isEmpty()) {
        lLastBucket = lBuckets.at(0);
        for (auto bucket : lBuckets) {
            if (bucket->created() > lLastBucket->created()) {
                lLastBucket = bucket;
            }
        }
    }

    return lLastBucket;
}
