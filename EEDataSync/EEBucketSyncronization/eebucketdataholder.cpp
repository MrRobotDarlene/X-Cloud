#include "eebucketdataholder.h"
#include <QDebug>

EEBucketDataHolder::EEBucketDataHolder(QObject *parent)
    : QObject(parent),
      mCurrentBucketIndex{-1},
      mCurrentBucket{nullptr}
{
}

EEBucketDataHolder::~EEBucketDataHolder()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (!mAllBuckets.isEmpty()) {
        qDeleteAll(mAllBuckets);
        mAllBuckets.clear();
    }
}


QList<EEBucket *> EEBucketDataHolder::allBuckets() const
{
    return mAllBuckets;
}

void EEBucketDataHolder::setAllBuckets(QList<EEBucket *> bucketList)
{
    if (!mAllBuckets.isEmpty()) {
        qDeleteAll(mAllBuckets);
        mAllBuckets.clear();
    }

    mCurrentBucketIndex = -1;
    mAllBuckets = bucketList;
}

void EEBucketDataHolder::addBucketToAllBuckets(EEBucket *newBucket)
{
    bool lIsAlreadyExists = false;
    qDebug() << "Bucket id: " << newBucket->id();

    foreach (EEBucket* bucket, mAllBuckets) {
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

void EEBucketDataHolder::setWorkingDirectory(QString &directory)
{
    mWorkingDirectory = directory;
}

QString EEBucketDataHolder::workingDirectory() const
{
    return mWorkingDirectory;
}

void EEBucketDataHolder::addFilesListByBucketId(QString bucketId, QList<EEFile> files)
{
    mBucketsFiles.insert(bucketId, files);
}

QList<EEFile> EEBucketDataHolder::filesByBucketId(QString bucketId)
{
    return mBucketsFiles[bucketId];
}

bool EEBucketDataHolder::setNextCurrentBucket()
{
    bool lIsNext = true;
    ++mCurrentBucketIndex;
    if (mCurrentBucketIndex < mAllBuckets.size()) {
        mCurrentBucket = mAllBuckets[mCurrentBucketIndex];
    } else {
        lIsNext = false;
        mCurrentBucketIndex = -1;
    }
    return lIsNext;
}


void EEBucketDataHolder::setCurrentFileIndex(int index)
{
    mCurrentFileIndex = index;
}

int EEBucketDataHolder::currentFileIndex() const
{
    return mCurrentFileIndex;
}

void EEBucketDataHolder::increaseCurrentIndex()
{
    ++mCurrentFileIndex;
}

void EEBucketDataHolder::setCurrentBucket(EEBucket *bucket)
{
    if (bucket != nullptr) {
        mCurrentBucket = bucket;

    } else {
        qDebug() << "Bucket wasn't initalizated! Keep use old current bucket...";
    }
}

EEBucket *EEBucketDataHolder::rootBucket() const
{
    QString lSeparator= "/";
#ifdef WIN32
    lSeparator = "\\";
#endif
    EEBucket *lRoot = nullptr;
    for (auto bucket : mAllBuckets) {
        if (bucket->name() == lSeparator) {
            lRoot = bucket;
            break;
        }
    }
    return lRoot;
}

EEBucket* EEBucketDataHolder::currentBucket() const
{
    return mCurrentBucket;
}

EEBucket* EEBucketDataHolder::bucketById(QString bucketId) const
{
    EEBucket* lBucket = nullptr;

    for (auto bucket : mAllBuckets) {
        if (bucket->id() == bucketId) {
            lBucket = bucket;
            break;
        }
    }
    return lBucket;
}

EEBucket* EEBucketDataHolder::bucketByName(QString bucketName) const
{
    EEBucket* lBucket = nullptr;

    for (auto bucket : mAllBuckets) {
        if (bucket->name() == bucketName) {
            lBucket = bucket;
            break;
        }
    }
    return lBucket;
}

bool EEBucketDataHolder::removeBucketByBucketId(QString bucketId)
{
    bool lIsRemoved = false;

    EEBucket* lBucket = nullptr;

    for (auto bucket : mAllBuckets) {
        if (bucket->id() == bucketId) {
            lBucket = bucket;
            break;
        }
    }

    if (lBucket != nullptr) {
        if (mAllBuckets.removeOne(lBucket)) {
            qDebug() << "Folder was succesfully removed from list!";
            delete lBucket;
            lIsRemoved = true;
        }
    }
    return lIsRemoved;
}
