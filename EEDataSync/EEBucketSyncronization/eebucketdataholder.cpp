#include "eebucketdataholder.h"
#include "EESDK//EEContainers/eefile.h"
#include "EESDK/EEContainers/eebucket.h"
#include "eefoldermodel.h"


#include <QDebug>

EEBucketDataHolder::EEBucketDataHolder(QObject *parent)
    : QObject(parent),
      mCurrentFileIndex{-1},
      mCurrentBucketIndex{-1},
      mCurrentBucket{nullptr},
      mBucketsFolderModelForm{nullptr}
{
}

EEBucketDataHolder::~EEBucketDataHolder()
{
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

void EEBucketDataHolder::addFilesListByBucketId(QString bucketId, QList<EEFile*> files)
{
    //clear previous data
    if (mBucketsFiles.contains(bucketId)) {
        QList<EEFile*>lBucketFiles = mBucketsFiles[bucketId];
        qDeleteAll(lBucketFiles);
        lBucketFiles.clear();
    }
    mBucketsFiles.insert(bucketId, files);
}

QList<EEFile *> EEBucketDataHolder::filesByBucketId(QString bucketId)
{
    return mBucketsFiles[bucketId];
}

EEFile *EEBucketDataHolder::bucketFileByName(QString fileName, QString bucketId)
{
     QList<EEFile*>lBucketFiles =  mBucketsFiles[bucketId];

     EEFile *lFile = nullptr;
     for (auto file:lBucketFiles) {
         if (file->filename() == fileName) {
             lFile = file;
             break;
         }
     }

     return lFile;
}

bool EEBucketDataHolder::removeFileFromBucket(QString fileName, QString bucketId)
{
    EEFile *lFile = bucketFileByName(fileName, bucketId);
    bool lIsRemoved = false;
    if ( mBucketsFiles[bucketId].removeOne(lFile)) {
        qDebug() << "File was removed from bucket succesfully";
        lIsRemoved = true;
    } else {
        qDebug() << "File wasn't removed from bucket!";
    }
    return lIsRemoved;

}

bool EEBucketDataHolder::removeBucketFromTreeById(QString bucketId)
{
    return mBucketsFiles.remove(bucketId);
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

/**
 * @brief EEBucketDataHolder::buildLocalFormFromBucket
 * Build EEFolderModel from EEBucket list
 * and write it into mBucketsFolderModelForm
 */
void EEBucketDataHolder::buildLocalFormFromBucket()
{
    QList<EEFolderModel*>lListOfFolders;
    QString lSeparator = "/";
    //set all of the buckets and their subfiles into list of the EEFolderModel
    for (auto bucket : mAllBuckets) {
        EEFolderModel *lFolderModel = new EEFolderModel;
        QString lBucketName = bucket->name();
        if (lBucketName.at(0) !=lSeparator) {
            lBucketName = lSeparator + lBucketName;
        }
        lFolderModel->setName(lBucketName);
        for (auto file :  mBucketsFiles[bucket->id()]) {
            EEModel *lModel = new EEModel;
            if (lFolderModel->name() != lSeparator) {
                lModel->setName(lFolderModel->name() + lSeparator + file->filename());
            } else {
                lModel->setName(lFolderModel->name() + file->filename());
            }
            lFolderModel->addFile(lModel);
        }
        lListOfFolders.append(lFolderModel);
    }


    //use reversal piramidal sorting
    //sort in desceding order by folder name
    //last element will be root
    for (size_t i(0); i < lListOfFolders.size(); ++i) {
        size_t lCurrentMaximum = lListOfFolders[i]->name().length();
        size_t lCurrentIndex = i;

        for (size_t j = i; j < lListOfFolders.size(); ++j) {
            if (lListOfFolders[j]->name().length() > lCurrentMaximum) {
                lCurrentMaximum = lListOfFolders[j]->name().length();
                lCurrentIndex = j;
            }
        }

        if (lCurrentIndex != i) {
            lListOfFolders.swap(i, lCurrentIndex);
        }
    }

    //algorithm, which search subfolders parent folder and add as subfolder to it
    //if subfolder has been added - break and move to next subfolder
    //last element is root
    //result of algorithm -  all of the subfolders setted to last element as subfolders
    for (auto subFolderIter = lListOfFolders.begin(); subFolderIter != lListOfFolders.end(); ++subFolderIter)  {
        for (auto baseFolderIter = lListOfFolders.begin(); baseFolderIter != lListOfFolders.end(); ++baseFolderIter) {
            QString lSubFolderName = (*subFolderIter)->name();
            QString lBaseFolderName = (*baseFolderIter)->name();
            if (lSubFolderName != lBaseFolderName) {
                //if subfolder's path contains parent's folder path
                if (lSubFolderName.contains(lBaseFolderName)) {
                    (*baseFolderIter)->addFolder(*subFolderIter);
                    break;
                }
            }
        }
    }

    //last element - root
    if (!lListOfFolders.isEmpty()) {
        mBucketsFolderModelForm = lListOfFolders.last();
    } else {
        qDebug() << "Root hasn't been initialized yet!";
    }
}

EEFolderModel *EEBucketDataHolder::bucketsFolderModelForm() const
{
    return mBucketsFolderModelForm;
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

void EEBucketDataHolder::setCurrentFileName(QString file)
{
    mCurrentFileName = file;
}


QString EEBucketDataHolder::currentFileName() const
{
    return mCurrentFileName;
}

EEBucket *EEBucketDataHolder::rootBucket() const
{
    QString lSeparator= "/";
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
