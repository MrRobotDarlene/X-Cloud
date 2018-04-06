#include "eebucketfacade.h"
#include "eefoldermodel.h"
#include "EESDK/EEContainers/eebucket.h"
#include "EESDK/EEContainers/eefile.h"

#include "eebucketdataholder.h"
#include "eebucketdownloadinghandler.h"
#include "eebucketuploadinghandler.h"
#include "eebucketupdatinghandler.h"

#include <QDebug>

EEBucketFacade::EEBucketFacade(QObject *parent)
    : QObject(parent),
      mBucketDataHolder{new EEBucketDataHolder()},
      mDownloadingHandler{new EEBucketDownloadingHandler(mBucketDataHolder)},
      mUploadingHandler{new EEBucketUploadingHandler(mBucketDataHolder)},
      mUpdatingHandler{new EEBucketUpdatingHandler(mBucketDataHolder)}
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
}

EEBucketFacade::~EEBucketFacade()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    delete mBucketDataHolder;
    delete mDownloadingHandler;
    delete mUpdatingHandler;
    delete mUploadingHandler;
}

void EEBucketFacade::addElementToUploadBucketQueue(EEFolderModel *folder)
{
    mUploadingHandler->addElementToUploadingBucketQueue(folder);
}

void EEBucketFacade::initializeDownloadingQueue()
{
    mDownloadingHandler->initializeDownloadingQueue();
}

QQueue<EEFolderModel *> EEBucketFacade::uploadingBucketsQueue() const
{
    return mUploadingHandler->uploadingBucketsQueue();
}

QQueue<EEBucket*> EEBucketFacade::downloadingBucketsQueue() const
{
    return mDownloadingHandler->downloadingBucketsQueue();
}

bool EEBucketFacade::setCurrentBucketByDownloadingQueue()
{
    return mDownloadingHandler->setCurrentBucketByDownloadingQueue();
}

void EEBucketFacade::setDownloadingBucketQueue(QList<EEBucket *> buckets)
{
    mDownloadingHandler->setDownloadingBucketQueue(buckets);
}

bool EEBucketFacade::removeBucketFromDownloadingQueue(EEBucket *bucket)
{
    return mDownloadingHandler->removeBucketFromDownloadingQueue(bucket);
}

bool EEBucketFacade::setCurrentFolderModelByUploadingBucket()
{
    return mUploadingHandler->setCurrentFolderModelByUploadingBucket();
}

void EEBucketFacade::setCurrentFileIndex(int index)
{
    mBucketDataHolder->setCurrentFileIndex(index);
}

int EEBucketFacade::currentFileIndex() const
{
    return mBucketDataHolder->currentFileIndex();
}

void EEBucketFacade::increaseCurrentIndex()
{
    mBucketDataHolder->increaseCurrentIndex();
}

EEFolderModel *EEBucketFacade::currentUploadingFolderModel() const
{
    return mUploadingHandler->currentFolderModel();
}

EEFolderModel *EEBucketFacade::currentUpdatingFolderModel() const
{
    return mUpdatingHandler->currentFolderModel();
}

QString EEBucketFacade::currentUploadingFolderName() const
{
    return mUploadingHandler->currentFolderName();
}

QString EEBucketFacade::currentUpdatingFolderName() const
{
    return mUpdatingHandler->currentFolderName();
}

int EEBucketFacade:: currentUploadingFolderSize() const
{
    return mUploadingHandler->currentFolderSize();
}

int EEBucketFacade::currentUpdatingFolderSize() const
{
    return mUpdatingHandler->currentFolderSize();
}

QString EEBucketFacade::currentUploadingFileNameFromFolderModel() const
{
    return mUploadingHandler->currentFileNameFromFolderModel();
}

bool EEBucketFacade::setCurrentFileByUploadingQueue()
{
    return mUploadingHandler->setCurrentFileByUploadingQueue();
}

EEFile EEBucketFacade::currentUploadingFile()
{
    return mUploadingHandler->currentUploadingFile();
}

void EEBucketFacade::addModelToUploadFilesQueue(QString fileName, QString bucketId)
{
    mUploadingHandler->addModelToUploadFilesQueue(fileName, bucketId);
}

QQueue<EEFile> EEBucketFacade::uploadingFilesQueue() const
{
    return mUploadingHandler->uploadingFilesQueue();
}

QString EEBucketFacade::currentUpdatingFileNameFromFolderModel() const
{
    return mUpdatingHandler->currentFileNameFromFolderModel();
}

QList<EEBucket*> EEBucketFacade::allBuckets() const
{
    return mBucketDataHolder->allBuckets();
}

void EEBucketFacade::setAllBuckets(QList<EEBucket*> bucketList)
{
    mBucketDataHolder->setAllBuckets(bucketList);
}

void EEBucketFacade::addBucketToAllBuckets(EEBucket *newBucket)
{
    mBucketDataHolder->addBucketToAllBuckets(newBucket);
}

void EEBucketFacade::setCurrentBucket(EEBucket *bucket)
{
    mBucketDataHolder->setCurrentBucket(bucket);
}

EEBucket* EEBucketFacade::currentBucket() const
{
    return mBucketDataHolder->currentBucket();
}

EEBucket *EEBucketFacade::bucketById(QString bucketId) const
{
    return mBucketDataHolder->bucketById(bucketId);
}

void EEBucketFacade::addFilesListByBucketId(QString bucketId, QList<EEFile> files)
{
    mBucketDataHolder->addFilesListByBucketId(bucketId, files);
}

QList<EEFile> EEBucketFacade::filesByBucketId(QString bucketId)
{
    return mBucketDataHolder->filesByBucketId(bucketId);
}

bool EEBucketFacade::setNextCurrentBucket()
{
    return mBucketDataHolder->setNextCurrentBucket();
}

bool EEBucketFacade::removeBucketByBucketId(QString bucketId)
{
    return mBucketDataHolder->removeBucketByBucketId(bucketId);
}

void EEBucketFacade::setOneBucketDownloadingQueueFiles(QList<EEFile> files)
{
    mDownloadingHandler->setOneBucketDownloadingQueueFiles(files);
}

bool EEBucketFacade::removeFileFromOneBucketDownloadingQueue(EEFile file)
{
    return mDownloadingHandler->removeFileFromOneBucketDownloadingQueue(file);
}

int EEBucketFacade::currentFilesListSize()
{
    return mDownloadingHandler->currentFilesListSize();
}

EEFile EEBucketFacade::currentFile() const
{
    return mDownloadingHandler->currentFile();
}

void EEBucketFacade::addCurrentFilesToDownloadQueue()
{
    mDownloadingHandler->addCurrentFilesToDownloadQueue();
}

bool EEBucketFacade::setCurrentFileByDownloadingQueue()
{
    return mDownloadingHandler->setCurrentFileByDownloadingQueue();
}

EEFile EEBucketFacade::currentDownloadingFile()
{
    return mDownloadingHandler->currentDownloadingFile();
}

QQueue<EEFile> EEBucketFacade::downloadingFileQueue() const
{
    return mDownloadingHandler->downloadingFileQueue();
}

void EEBucketFacade::setWorkingDirectory(QString directory)
{
    mBucketDataHolder->setWorkingDirectory(directory);
}

QString EEBucketFacade::workingDirectory() const
{
    return mBucketDataHolder->workingDirectory();
}

EEBucket* EEBucketFacade::rootBucket() const
{
    return mBucketDataHolder->rootBucket();
}

EEBucket *EEBucketFacade::lastBucket() const
{
    return mUpdatingHandler->lastBucket();
}

void EEBucketFacade::addBucketToDownloadingQueue(EEBucket* bucket)
{
    mDownloadingHandler->addBucketToDownloadingQueue(bucket);
}

void EEBucketFacade::addBucketToUploadingQueue(EEFolderModel* folder)
{
    mUploadingHandler->addElementToUploadingBucketQueue(folder);
}

EEBucket *EEBucketFacade::bucketByName(QString bucketName) const
{
    EEBucket* lBucket = nullptr;

    auto lAllBuckets = mBucketDataHolder->allBuckets();

    QString lSeparator;
#ifdef WIN32
    lSeparator = "\\";
#else
     lSeparator = "/";
#endif
    for (auto bucket : lAllBuckets) {
        if (bucket->name() == bucketName) {
            lBucket = bucket;
            break;
        }

        if (lSeparator + bucket->name() == bucketName) {
            lBucket = bucket;
            break;
        }
    }
    return lBucket;
}

QQueue<EEFolderModel *> EEBucketFacade::updateBucketQueue() const
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    return mUpdatingHandler->updatingBucketQueue();
}

void EEBucketFacade::addElementToUpdateBucketQueue(EEFolderModel *folder)
{
    mUpdatingHandler->addElementToUpdateBucketQueue(folder);
}

void EEBucketFacade::addFileToUpdateFileQueue(EEFile file)
{
    mUpdatingHandler->addFileToUpdateFileQueue(file);
}

QQueue<EEFile> EEBucketFacade::updatingFilesQueue()
{
    return mUpdatingHandler->updatingFilesQueue();
}

bool EEBucketFacade::setCurrentFileByUpdatingQueue()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    return mUpdatingHandler->setCurrentFileByUpdatingQueue();
}

EEFile EEBucketFacade::currentUpdatingFile()
{
    return mUpdatingHandler->currentUpdatingFile();
}

void EEBucketFacade::initializeUploadingBucketsQueueFromUpdateQueue()
{
    mUploadingHandler->setUploadingBucketQueue(mUpdatingHandler->updatingBucketQueue());
}

bool EEBucketFacade::setCurrentFolderModelByUpdatingBucket()
{
    return mUpdatingHandler->setCurrentFolderModelByUpdatingBucket();
}

bool EEBucketFacade::setCurrentBucketsQueueByUpdatingBucket()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (!mDownloadingHandler->downloadingBucketsQueue().isEmpty()) {
        mDownloadingHandler->downloadingBucketsQueue().clear();
    }

    for (auto bucket : mUpdatingHandler->uploadingBucketsQueue()) {
        mDownloadingHandler->addBucketToDownloadingQueue(bucketByName(bucket->name()));
    }

}

bool EEBucketFacade::setUpdateFilesQueueToUploadQueue()
{
    mUploadingHandler->addQueueToUploadingFileQueue(mUpdatingHandler->updatingFilesQueue());
}
