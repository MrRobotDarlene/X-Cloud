#include "eebucketwrapper.h"
#include "eefoldermodel.h"
#include "EESDK/EEContainers/eebucket.h"
#include <QDebug>

EEBucketWrapper::EEBucketWrapper(QObject *parent)
    : QObject(parent),
      mCurrentFolderModel{nullptr}
{

}

void EEBucketWrapper::addElementToUploadBucketQueue(EEFolderModel *folder)
{
    mUploadingBucketQueue.append(folder);
}

void EEBucketWrapper::initializeDownloadingQueue()
{
    mDownloadingBucketQueue.append(mAllBuckets);
}

QQueue<EEFolderModel *> EEBucketWrapper::uploadingBucketsQueue() const
{
    return mUploadingBucketQueue;
}

QQueue<EEBucket> EEBucketWrapper::downloadingBucketsQueue() const
{
    return mDownloadingBucketQueue;
}

bool EEBucketWrapper::setCurrentBucketByDownloadingQueue()
{
    bool lIsBucketSetted = false;
    if (!mDownloadingBucketQueue.isEmpty()) {
        mCurrentBucket = mDownloadingBucketQueue.dequeue();
        lIsBucketSetted = true;
    }
    return lIsBucketSetted;
}

bool EEBucketWrapper::setCurrentFolderModelByBucket()
{
    bool lIsBucketSetted = false;
    if (!mUploadingBucketQueue.isEmpty()) {
        mCurrentFolderModel = mUploadingBucketQueue.dequeue();
        lIsBucketSetted = true;
    }
    return lIsBucketSetted;
}

void EEBucketWrapper::setCurrentFileIndex(int index)
{
    mCurrentFileIndex = index;
}

int EEBucketWrapper::currentFileIndex() const
{
    return mCurrentFileIndex;
}

void EEBucketWrapper::increaseCurrentIndex()
{
    ++mCurrentFileIndex;
}

EEFolderModel *EEBucketWrapper::currentFolderModel() const
{
    return mCurrentFolderModel;
}

QString EEBucketWrapper::currentFolderName() const
{
    return mCurrentFolderModel->name();
}

int EEBucketWrapper::currentFolderSize() const
{
    return mCurrentFolderModel->getFilesList().size();
}

QString EEBucketWrapper::currentFileNameFromFolderModel() const
{
    return mCurrentFolderModel->getFilesList().at(mCurrentFileIndex)->name();
}

QList<EEBucket> EEBucketWrapper::allBuckets() const
{
    return mAllBuckets;
}

void EEBucketWrapper::setAllBuckets(QList<EEBucket> bucketList)
{
    mAllBuckets = bucketList;
}

void EEBucketWrapper::addBucketToAllBuckets(EEBucket newBucket)
{
    bool lIsAlreadyExists = false;
    qDebug() << "Bucket id: " << newBucket.id();

    foreach (EEBucket bucket, mAllBuckets) {
        if (bucket == newBucket) {
            qDebug() << "Bucket is already on the list";
            lIsAlreadyExists = true;
            break;
        }
    }

    if (!lIsAlreadyExists) {
        qDebug() << "Adding new bucket to the list of buckets!";
        mAllBuckets.append(newBucket);
    }
}

void EEBucketWrapper::setCurrentBucket(EEBucket bucket)
{
    mCurrentBucket = bucket;
}

EEBucket EEBucketWrapper::currentBucket() const
{
    return mCurrentBucket;
}

EEBucket EEBucketWrapper::bucketById(QString bucketId) const
{
    EEBucket lBucket;

    for (auto bucket : mAllBuckets) {
        if (bucket.id() == bucketId) {
            lBucket = bucket;
            break;
        }
    }
    return lBucket;
}

void EEBucketWrapper::setCurrentFileList(QList<EEFile> files)
{
    mCurrentFileList = files;
}

int EEBucketWrapper::currentFilesListSize()
{
    return mCurrentFileList.size();
}

EEFile EEBucketWrapper::currentFile() const
{
    return mCurrentFileList.at(mCurrentFileIndex);
}

void EEBucketWrapper::setRestoreDirectory(QString directory)
{
    mRestoreDirectory = directory;
}

QString EEBucketWrapper::restoreDirectory() const
{
    return mRestoreDirectory;
}
