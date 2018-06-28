#include "eebucketfacade.h"
#include "EEDataSync/EELocalDataParser/eefoldermodel.h"
#include "EESDK/EEContainers/eebucket.h"
#include "EESDK/EEContainers/eefile.h"

#include "eebucketdataholder.h"
#include "eebucketdownloadinghandler.h"
#include "eebucketuploadinghandler.h"
#include "eebucketupdatinghandler.h"
#include "eebucketdeletionhandler.h"
#include "eeoutdateddataholder.h"
#include "eebaseoutdateddataholder.h"

#include <QDebug>
#include <typeinfo>

EEBucketFacade::EEBucketFacade(QObject *parent)
    : QObject(parent),
      mBucketDataHolder{new EEBucketDataHolder()},
      mDownloadingHandler{new EEBucketDownloadingHandler(mBucketDataHolder)},
      mUploadingHandler{new EEBucketUploadingHandler(mBucketDataHolder)},
      mUpdatingHandler{new EEBucketUpdatingHandler(mBucketDataHolder)},
      mBucketDeletionHandler{new EEBucketDeletionHandler{mBucketDataHolder}},
      mOutdatedDataHolder{new EEOutdatedDataHolder},
      mCorruptedDataHolder{new EEBaseOutdatedDataHolder}
{
}

EEBucketFacade::~EEBucketFacade()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    delete mCorruptedDataHolder;
    delete mOutdatedDataHolder;

    delete mBucketDeletionHandler;

    delete mBucketDataHolder;
    delete mDownloadingHandler;
    delete mUpdatingHandler;
    delete mUploadingHandler;
}

void EEBucketFacade::addElementToUploadBucketQueue(EEFolderModel *folder)
{
    mUploadingHandler->addElementToBucketQueue(folder);
}

void EEBucketFacade::initializeDownloadingQueue()
{
    mDownloadingHandler->initializeDownloadingQueue();
}

QQueue<EEFolderModel *> EEBucketFacade::uploadingBucketsQueue() const
{
    return mUploadingHandler->bucketsQueue();
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
    return mUploadingHandler->setCurrentFolderModelByBucket();
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
    return mUploadingHandler->setCurrentFileQueueByQueue();
}

EEFile *EEBucketFacade::currentUploadingFile()
{
    return mUploadingHandler->currentFile();
}

void EEBucketFacade::addModelToUploadFilesQueue(QString fileName, QString bucketId)
{
    mUploadingHandler->addModelToFilesQueue(fileName, bucketId);
}

QQueue<EEFile*> EEBucketFacade::uploadingFilesQueue() const
{
    return mUploadingHandler->filesQueue();
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

void EEBucketFacade::setCurrentFileName(QString file)
{
    mBucketDataHolder->setCurrentFileName(file);
}

QString EEBucketFacade::currentFileName() const
{
    return mBucketDataHolder->currentFileName();
}

void EEBucketFacade::addFilesListByBucketId(QString bucketId, QList<EEFile*> files)
{
    mBucketDataHolder->addFilesListByBucketId(bucketId, files);
}

QList<EEFile*> EEBucketFacade::filesByBucketId(QString bucketId)
{
    return mBucketDataHolder->filesByBucketId(bucketId);
}

EEFile *EEBucketFacade::bucketFileByName(QString fileName, QString bucketId)
{
    return mBucketDataHolder->bucketFileByName(fileName, bucketId);
}

bool EEBucketFacade::removeFileFromBucket(QString fileName, QString bucketId)
{
    return mBucketDataHolder->removeFileFromBucket(fileName, bucketId);
}

bool EEBucketFacade::setNextCurrentBucket()
{
    return mBucketDataHolder->setNextCurrentBucket();
}

bool EEBucketFacade::removeBucketByBucketId(QString bucketId)
{
    return mBucketDataHolder->removeBucketByBucketId(bucketId);
}

bool EEBucketFacade::removeBucketFromTreeById(QString bucketId)
{
    return mBucketDataHolder->removeBucketFromTreeById(bucketId);
}

void EEBucketFacade::buildLocalFormFromBucket()
{
    mBucketDataHolder->buildLocalFormFromBucket();
}

EEFolderModel *EEBucketFacade::bucketsFolderModelForm() const
{
    return mBucketDataHolder->bucketsFolderModelForm();
}

void EEBucketFacade::setTemporaryDownloadingQueueFiles(QList<EEFile*> files)
{
    mDownloadingHandler->setTemporaryDownloadingQueueFiles(files);
}

bool EEBucketFacade::removeFileFromTemporaryDownloadingQueue(EEFile *file)
{
    return mDownloadingHandler->removeFileFromTemporaryDownloadingQueue(file);
}

int EEBucketFacade::temporaryBucketFilesListSize()
{
    return mDownloadingHandler->temporaryBucketFilesListSize();
}

EEFile *EEBucketFacade::temporaryBucketCurrentFile() const
{
    return mDownloadingHandler->temporaryBucketCurrentFile();
}

void EEBucketFacade::addCurrentFilesToDownloadQueue()
{
    mDownloadingHandler->addFilesListToQueue();
}

bool EEBucketFacade::setCurrentFileByDownloadingQueue()
{
    return mDownloadingHandler->setCurrentFileQueueByQueue();
}

EEFile *EEBucketFacade::currentDownloadingFile()
{
    return mDownloadingHandler->currentFile();
}

QQueue<EEFile*> EEBucketFacade::downloadingFileQueue() const
{
    return mDownloadingHandler->filesQueue();
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
    mUploadingHandler->addElementToBucketQueue(folder);
}

EEBucket *EEBucketFacade::bucketByName(QString bucketName) const
{
    EEBucket* lBucket = nullptr;

    auto lAllBuckets = mBucketDataHolder->allBuckets();

    QString lSeparator =  "/";
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

    return mUpdatingHandler->bucketsQueue();
}

void EEBucketFacade::addElementToUpdateBucketQueue(EEFolderModel *folder)
{
    mUpdatingHandler->addElementToBucketQueue(folder);
}

void EEBucketFacade::addFileToUpdateFileQueue(EEFile *file)
{
    mUpdatingHandler->addFileToFileQueue(file);
}

QQueue<EEFile*> EEBucketFacade::updatingFilesQueue()
{
    return mUpdatingHandler->filesQueue();
}

bool EEBucketFacade::setCurrentFileByUpdatingQueue()
{
    return mUpdatingHandler->setCurrentFileQueueByQueue();
}

EEFile *EEBucketFacade::currentUpdatingFile()
{
    return mUpdatingHandler->currentFile();
}

void EEBucketFacade::addToDeletionBucketQueue(EEBucket *bucket)
{
    mBucketDeletionHandler->addToDeletionBucketQueue(bucket);
}

EEBucket *EEBucketFacade::nextBucketToRemove()
{
    return mBucketDeletionHandler->nextBucketToRemove();
}

void EEBucketFacade::addToDeletionFilesList(EEFile *file)
{
    mBucketDeletionHandler->addFileToFileQueue(file);
}

bool EEBucketFacade::setCurrentFileByDeletionQueue()
{
    return mBucketDeletionHandler->setCurrentFileQueueByQueue();
}

EEFile *EEBucketFacade::currentDeletionFile()
{
    return mBucketDeletionHandler->currentFile();
}

void EEBucketFacade::initializeUploadingBucketsQueueFromUpdateQueue()
{
    mUploadingHandler->setBucketQueue(mUpdatingHandler->bucketsQueue());
}

bool EEBucketFacade::setCurrentFolderModelByUpdatingBucket()
{
    return mUpdatingHandler->setCurrentFolderModelByBucket();
}

void EEBucketFacade::setCurrentBucketsQueueByUpdatingBucket()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (!mDownloadingHandler->downloadingBucketsQueue().isEmpty()) {
        mDownloadingHandler->downloadingBucketsQueue().clear();
    }

    for (auto bucket : mUpdatingHandler->bucketsQueue()) {
        mDownloadingHandler->addBucketToDownloadingQueue(bucketByName(bucket->name()));
    }

}

void EEBucketFacade::setUpdateFilesQueueToUploadQueue()
{
    mUploadingHandler->addQueueToFileQueue(mUpdatingHandler->filesQueue());
}

void EEBucketFacade::addOutdatedCloudFile(EEFile *file)
{
    mOutdatedDataHolder->addCloudFile(file);
}

void EEBucketFacade::addOutdatedLocalFile(QString fileName)
{
    mOutdatedDataHolder->addLocalFile(fileName);
}

void EEBucketFacade::addOutdateFolderToRemoveList(EEFolderModel *folderModel)
{
    mOutdatedDataHolder->addOutdateFolderToRemoveList(folderModel);
}

QList<EEFolderModel *> EEBucketFacade::outdatedLocalFolders() const
{
    return mOutdatedDataHolder->outdatedLocalFolders();
}

QList<EEFile *> EEBucketFacade::outDatedCloudFiles() const
{
    return mOutdatedDataHolder->cloudFilesToRemove();
}

EEFile *EEBucketFacade::firstOutDateElement() const
{
    return mOutdatedDataHolder->firstElement();
}

void EEBucketFacade::removeFirstOutdated()
{
    mOutdatedDataHolder->removeFirst();
}

QList<EEFolderModel *> EEBucketFacade::outDatedLocalFolderWithFilesToRemove() const
{
    return mOutdatedDataHolder->localFolderWithFilesToRemove();
}

void EEBucketFacade::clearOutDatedFiles()
{
    mOutdatedDataHolder->clearLocalValues();
}

void EEBucketFacade::clearOutDatedFolderList()
{
    mOutdatedDataHolder->clearOutDatedFolderList();
}

void EEBucketFacade::addCorruptedCloudFile(EEFile *file)
{
    mCorruptedDataHolder->addCloudFile(file);
}

void EEBucketFacade::addCorruptedLocalFile(QString fileName)
{
    mCorruptedDataHolder->addLocalFile(fileName);
}

QList<EEFile *> EEBucketFacade::corruptedCloudFiles() const
{
    return mCorruptedDataHolder->cloudFilesToRemove();
}

EEFile *EEBucketFacade::firstCorruptedElement() const
{
    return mCorruptedDataHolder->firstElement();
}

void EEBucketFacade::removeFirstCorrupted()
{
    mCorruptedDataHolder->removeFirst();
}

QList<EEFolderModel *> EEBucketFacade::corruptedFoldersWithFilesToRemove() const
{
    return mCorruptedDataHolder->localFolderWithFilesToRemove();
}

void EEBucketFacade::clearCorruptedLocalValues()
{
    mCorruptedDataHolder->clearLocalValues();
}
