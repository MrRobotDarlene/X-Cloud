#include "eedatamanager.h"
#include "eefolderparsecontroller.h"
#include "eefoldermodel.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "eestatustimer.h"

#include "EESDK/EEContainers/eebucket.h"

#include "EESDK/eesdk.h"
#include "EESDK/eefileloader.h"
#include "EESDK/EEContainers/eefile.h"
#include "eedatatreebuilder.h"
#include "eedatetimesyncronizator.h"
#include "eedatacomparator.h"
#include "eedataloader.h"
#include "eedataremover.h"

#include "crypto.h"
#include "controller.h"
#include "eejsonbuilder.h"
#include "eejsonvalidator.h"
#include "eejsonreader.h"

#include <QFile>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDateTime>

#include <typeinfo>


EEDataManager::EEDataManager(EESDK *sdk, EEFileLoader *loader, QString path, QObject *parent) :
    QObject(parent),
    mSdk{sdk},
    mLoader{loader},
    mFolderParseController{new EEFolderParseController},
    mBucketFacade{new EEBucketFacade},
    mJsonBuilder{new EEJsonBuilder(this)},
    mJsonValidator{new EEJsonValidator(mBucketFacade, mJsonBuilder, this)},
    mJsonReader{new EEJsonReader(mJsonBuilder)},
    mDataTreeBuilder{new EEDataTreeBuilder(mSdk, mBucketFacade, this)},
    mDateTimeSyncronizator{new EEDateTimeSyncronizator(mSdk, mJsonBuilder, mBucketFacade, mFolderParseController, this)},
    mDataComparator{new EEDataComparator(mSdk, mBucketFacade, mFolderParseController, this)},
    mDataLoader{new EEDataLoader(mSdk, mJsonBuilder, mBucketFacade, mFolderParseController, mLoader, this)},
    mDataRemover{new EEDataRemover(mSdk, mBucketFacade, mJsonValidator, mLoader, this)},
    mCurrentState{EECurrentBucketsState::UnknownState}
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->setWorkingDirectory(path);

    connect(mSdk, SIGNAL(bucketsReceived(QList<EEBucket*>)),
            this, SLOT(handleBucketsListReceive(QList<EEBucket*>)));
    connect(mSdk, SIGNAL(filesForBucketReceived(QList<EEFile*>)),
            this, SLOT(handleFilesInfoReceived(QList<EEFile*>)));
    connect(mSdk, SIGNAL(bucketCreated(EEBucket*)),
            mDataLoader, SLOT(bucketCreated(EEBucket*)));
    connect(mSdk, SIGNAL(bucketDeleted(QString)),
            this, SLOT(handleBucketDeletion()));

    connect(mLoader, SIGNAL(fileDownloaded()),
            mDateTimeSyncronizator, SLOT(synchronizateFileDateTime()));

    connect(mLoader, SIGNAL(fileDowloadingError()),
            this,SLOT(handleContinueNextProcess()));

    connect(mLoader, SIGNAL(fileDeleted()),
            this, SLOT(handleFileDeletion()));

    connect(mLoader, SIGNAL(fileDeletionError()),
            this,SLOT(handleContinueNextProcess()));

    connect(mLoader, SIGNAL(fileUploaded()),
            mDateTimeSyncronizator, SLOT(initializeFileDateSyncronization()));

    connect(mLoader, SIGNAL(fileUploadingError()),
            this,SLOT(handleContinueNextProcess()));

    connect(mDateTimeSyncronizator, SIGNAL(fileDateTimeSyncronizated()),
            this, SLOT(handleContinueNextProcess()));

    connect(&EEStatusTimer::getInstance(),&EEStatusTimer::checkStatusByTimer,
            mDataTreeBuilder, [this] () {
        setCurrentState(EECurrentBucketsState::FileStatusCheck);
        mDataTreeBuilder->startBucketsStatusCheck();
    });

    connect(mDataTreeBuilder, SIGNAL(cloudTreeReceived()), this, SLOT(checkCorruptionDataStatus()));
    connect(mDataRemover, SIGNAL(corruptedDataRemoved()), this, SLOT(checkDeletionDataStatus()));

    connect(mDataLoader, SIGNAL(dataDeleted()), this, SLOT(checkDataActuality()));
    connect(mDataLoader, &EEDataLoader::startFilesUploading, this, [this]() {
        qDebug() << "All neccesary files removed! Start to upload...";
        setCurrentState(EECurrentBucketsState::ExistingBucketFilesUploading);
        //start to upload data for setted before upload list
        mDataLoader->uploadNextFiles();
    });

    connect(mDataLoader, &EEDataLoader::uploadingFinished, this, [this]() {
        qDebug() << "Uploading finished!"
                    "Start to download files...";
        setCurrentState(EECurrentBucketsState::NewBucketFilesDownloading);
        mDataLoader->useNextBucketForDownloading();
    });

    connect(mDataLoader, &EEDataLoader::startFilesDownloading, this, [this]() {
        qDebug() << "All new buckets files were downloaded!"
                    "Start to download files for existing...";
        setCurrentState(EECurrentBucketsState::ExistingBucketFilesDownloading);
        mDataLoader->cloudFilesDownloading();
    });

    connect(mDataComparator, SIGNAL(dataCompared()), this, SLOT(startDataUpdate()));
}

EEDataManager::~EEDataManager()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (mFolderParseController != nullptr) {
        delete mFolderParseController;
    }
    delete mBucketFacade;
}
/**
 * @brief EEDataManager::start
 * Start timer first time
 */
void EEDataManager::start()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEStatusTimer::getInstance().run();
}
/**
 * @brief EEDataManager::initTimerForDataCheck
 * Initialize timer, which will check status each 24 hours
 * Also, check buckets in the beggind of the buckets run
 */
void EEDataManager::restartTimerForDataCheck()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEStatusTimer::getInstance().startTimer();
}
/**
* @brief EEDataManager::stopTimerForDataCheck
* Stops timer, which was checking status each 1 minute
*/
void EEDataManager::stopTimerForDataCheck()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEStatusTimer::getInstance().stopCheck();
}
/**
 * @brief isTimerRunning
 * Check is timer running right now
 * @return
 */
bool EEDataManager::isTimerRunning() const
{
    return EEStatusTimer::getInstance().isRunning();
}

/**
 * @brief EEDataManager::initStorage
 * Build bucket queue from root folder
 * then start to upload them to the cloud
 * @param folder
 */
void EEDataManager::initStorage()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mFolderParseController->startSubfoldersInitialization(mBucketFacade->workingDirectory());

    EEFolderModel *mRoot = mFolderParseController->rootModel();

    setCurrentState(EECurrentBucketsState::NewBucketsFileUploading);
    mBucketFacade->addElementToUploadBucketQueue(mRoot);
    buildUploadingDataQueue(mRoot);

    mDataLoader->createNextBucket();
}
/**
 * @brief EEDataManager::initLocalFromCloud
 * Initialize process of data donwloading to local pc
 * @param folder
 */
void EEDataManager::initLocalFromCloud()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->setWorkingDirectory(mBucketFacade->workingDirectory());
    setCurrentState(EECurrentBucketsState::NewBucketFilesDownloading);

    mSdk->getBuckets();
}
/**
 * @brief EEDataManager::removeCorruptedValueAccordingToJson
 * Control process of corrupted files buiding and begin process of their removing
 */
void EEDataManager::removeCorruptedValueAccordingToJson()
{
    qDebug() << "- EEDataManager::removeCorruptedValueAccordingToJson()";
    mJsonValidator->initializeBuildCorruptedFilesList();

    //remove local
    mDataRemover->removeLocalCorrupted();
    //remove from json
    mJsonValidator->removeCorruptedFilesFromJson();

    //remove from cloud and change json status
    setCurrentState(EECurrentBucketsState::CorruptedDataDeletion);
    mDataRemover->removeNextCloudFile();
}
/**
 * @brief EEDataManager::deleteBucketsFromCloud
 * Delete all data from cloud and start to upload local
 */
void EEDataManager::deleteBucketsFromCloud()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    qDebug() << "Destroying buckets...";
    foreach (EEBucket *bucket, mBucketFacade->allBuckets()) {
        qDebug() << bucket->id() << " " << "Destroying buckets";
        mSdk->destroyBucket(bucket->id());
    }

    initStorage();
}
/**
 * @brief EEDataManager::initializeDeletionProcess
 * Start process of data deletion from cloud
 * It makes to upload new local data there
 */
void EEDataManager::initializeOutdatedFilesClear()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    setCurrentState(EECurrentBucketsState::CloudBucketsClear);

    mSdk->getBuckets();
}
/**
 * @brief EEDataManager::buildDataQueue
 * Build queue of buckets. Using in order for folder tree
 * Folder equal to  bucket
 * @param folder
 */
void EEDataManager::buildUploadingDataQueue(EEFolderModel * const folder)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    foreach (EEFolderModel *folder, folder->folderList()) {
        mBucketFacade->addElementToUploadBucketQueue(folder);
        buildUploadingDataQueue(folder);
    }
}

/**
 * @brief EEDataManager::checkCloudDataStatus
 * Check is json created
 * If no - go directly to check data actuality
 * if yes - check for corrupted data and start to remove it from cloud and json
 */
void EEDataManager::checkCorruptionDataStatus()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mFolderParseController->startSubfoldersInitialization(mBucketFacade->workingDirectory());

    if (mJsonBuilder->isJsonExists()) {
        qDebug() << "Json is already exists!";
        removeCorruptedValueAccordingToJson();
    } else {
       qDebug() << "Json with files/folders information is not exists yet - create it";
       checkDataActuality();
    }
}
/**
 * @brief EEDataManager::checkDeletionDataStatus
 * Check is some of data is outdated
 * Remove folder/file, if needed
 */
void EEDataManager::checkDeletionDataStatus()
{
    qDebug() << "- EEDataManager::checkDeletionDataStatus()";
    //read of the cloud data into eefolder form
    mBucketFacade->buildLocalFormFromBucket();

    //read json data into eefolderform
    mJsonReader->initializeReadingJsonIntoFolderModel();
    //compare local, json and cloud data
    mDataComparator->initializeOutdatedDataComparation(mJsonReader->jsonInFolderModelForm(),
                                                       mBucketFacade->bucketsFolderModelForm());
    setCurrentState(EECurrentBucketsState::OutdatedDataDeletion);

    //remove local folders
    mDataRemover->removeOutDatedLocalFolder();
    //remove folder from json
    mJsonValidator->removeOutDatedFoldersFromJson();
    //remove local files
    mDataRemover->removeLocalOutDated();
    //remove from json
    mJsonValidator->removeOutDatedFilesFromJson();
    //delete buckets/files from bucket
    mDataLoader->deleteNextBucket();
}
/**
 * @brief EEDataManager::checkDataActuality
 * if local data has even been initialized before - download all the data from cloud
 * if cloud data hasn't been initalized yet or been initialized just part of data - delete it and upload all of the local
 * In others cases - start to compare all of the folders and files
 */
void EEDataManager::checkDataActuality()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEBucket* lCloudLastFolder = mBucketFacade->rootBucket();

    if (lCloudLastFolder != nullptr ) {
        mFolderParseController->startSubfoldersInitialization(mBucketFacade->workingDirectory());
        EEFolderModel *lLocalFolderRoot = mFolderParseController->rootModel();
        bool lIsLocalCorrect = false;
        if (lLocalFolderRoot != nullptr) {
            if (!lLocalFolderRoot->isEmpty()) {
                qDebug() << "Directory isn't empty";
                lIsLocalCorrect = true;
            } else {
                //if local is empty
                qDebug() << "Directory is empty!";
            }
        } else {
            //if local hasn't been initialized yet
            qDebug() << "No information about local data  Try to download it first";
        }

        if (lIsLocalCorrect) {
            qDebug() << "LOCAL FOLDER IS NOT EMPTY AND EXISTS - START TO COMPARE DATA!!!";
            mDataComparator->startCompareData();
        } else {
            qDebug() << "Local directory is empty!!! Start to download all the data from cloud...";
            initLocalFromCloud();
        }

    } else {
        //if cloud data hasn't been initalized or initalized wrong way
        qDebug() << "No information about root folder on the cloud! Try to upload it first";
        initializeOutdatedFilesClear();
    }
}

/**
 * @brief EEDataManager::initDataUpdate
 * After all neccesary folder/files for donwloading/uploading were built
 * start processes required for each of them
 */
void EEDataManager::startDataUpdate()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    setCurrentState(EECurrentBucketsState::FileUpdating);

    mDataLoader->startDataUpdate();
}

/**
 * @brief EEDataManager::setCurrentState
 * Set current program state
 * @param state
 */
void EEDataManager::setCurrentState(EECurrentBucketsState state)
{
    mCurrentState = state;
}

/**
 * @brief EEDataManager::handleBucketDeletion
 * deletion possibly can be called in 3 cases:
 *  files updating, removing outdated buckets or removing all of the files from cloud(if root doesn't exist)
 */
void EEDataManager::handleBucketDeletion()
{
    if (mCurrentState == EECurrentBucketsState::FileUpdating) {
        mJsonBuilder->updateCurrentElementStatus();
        mDataLoader->updateNextBucket();
    } else if (mCurrentState == EECurrentBucketsState::OutdatedDataDeletion) {
        //remove from json
        qDebug() << "Try to remove folder from json: " << mBucketFacade->currentBucket()->name();
        mJsonBuilder->initializeRemoveObjectFromJson(mBucketFacade->currentBucket()->name());

        //remove bucket from cloud treew
        QString lBucketId = mBucketFacade->currentBucket()->id();
        mBucketFacade->removeBucketByBucketId(lBucketId);
        mBucketFacade->removeBucketFromTreeById(lBucketId);

        mDataLoader->deleteNextBucket();
    }
}

/**
 * @brief EEDataManager::handleFileDeletion
 * If updating process is on - keep removing needed files from updating
 * If corrupted data deletion - update status in json and begin to remove next file
 * if outdated data deletion - the same as with corrupted, but local files
 */
void EEDataManager::handleFileDeletion()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (mCurrentState == EECurrentBucketsState::FileUpdating) {
        //remove file from tree
        mBucketFacade->removeFileFromBucket(mBucketFacade->currentUpdatingFile()->filename(),
                                            mBucketFacade->currentUpdatingFile()->bucketId());
        mDataLoader->cloudFilesUpdating();
    } else if (mCurrentState == EECurrentBucketsState::CorruptedDataDeletion) {
        //remove from tree of cloud data
        mBucketFacade->removeFileFromBucket(mBucketFacade->firstCorruptedElement()->filename(),
                                            mBucketFacade->firstCorruptedElement()->bucketId());
        //update corrupted element status in json
        mJsonValidator->updateCorruptedFirstElementStatus();
        //keep updating next file
        mDataRemover->removeNextCloudFile();
    } else if (mCurrentState == EECurrentBucketsState::OutdatedDataDeletion) {
        EEBucket *lBucket = mBucketFacade->bucketById(mBucketFacade->currentDeletionFile()->bucketId());
        QString lFileName = mBucketFacade->currentDeletionFile()->filename();
        //remove from cloud tree
        mBucketFacade->removeFileFromBucket(lFileName, lBucket->id());
        //remove from json
        mJsonBuilder->initializeRemoveObjectFromJson(lBucket->name(), lFileName);
        //move to the next file for deletion
        mDataLoader->deleteNextFile();
    }
}

/**
 * @brief EEDataManager::handleBucketsListReceive
 * After receiving of the buckets list,
 * it could possibly make different actions depends on current state
 * @param buckets
 */
void EEDataManager::handleBucketsListReceive(QList<EEBucket*> buckets)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->setAllBuckets(buckets);
    if (mCurrentState == EECurrentBucketsState::FileUpdating) {
        qDebug() << "Updating buckets list";
    } else if (mCurrentState == EECurrentBucketsState::NewBucketsFileUploading) {
        qDebug() << "Uploading buckets list";
    } else if (mCurrentState == EECurrentBucketsState::NewBucketFilesDownloading) {
        qDebug() << "Downloading buckets list";
        mDataLoader->bucketsListReceived();
    } else if (mCurrentState == EECurrentBucketsState::FileStatusCheck) {
        qDebug() << "Check status of files!";
        mDataTreeBuilder->getFilesInfoForNextBucket();
    } else if (mCurrentState == EECurrentBucketsState::CloudBucketsClear){
        deleteBucketsFromCloud();
    } else {
        qDebug() << "UNKNOWN STATE!";
    }
}

/**
 * @brief EEDataManager::handleFilesInfoReceived
 * Slot, which activates after receiving list of file for current bucket
 * then decrypt list of files names
 * using .c decryption
 * @param files - list of files
 */
void EEDataManager::handleFilesInfoReceived(QList<EEFile*> files)
{
    for (auto &file : files) {
        QString lFileName = file->filename();
        EEBucket* lFileBucket = mBucketFacade->bucketById(file->bucketId());
        if (lFileBucket == nullptr) {
            qDebug() << "Error! Bucket is not initialized!!!" << file->bucketId();
            return;
        }

        //neccesary for casting corectness
        QByteArray lMnenmonicBa = lFileBucket->encryptionKey().toLatin1();
        QByteArray lBucketIdBa = lFileBucket->id().toUpper().toLatin1();
        QByteArray lFileNameBa = lFileName.toLatin1();

        const char *lMnenmonic = lMnenmonicBa.constData();
        const char *lBucketId = lBucketIdBa.constData();
        const char *lFileNameChar = lFileNameBa.constData();
        char *result;

        if (!decrypt_file_name(lMnenmonic, lBucketId, lFileNameChar, &result)) {
            lFileName = QString::fromUtf8(result);
        } else {
            qDebug() << "File name couldn't be decrypted!";
            lFileName = "Unknown_Name";
        }
        file->setFilename(lFileName);
    }

    //add all cloud files from bucket to map
    mBucketFacade->addFilesListByBucketId(mBucketFacade->currentBucket()->id(), files);

    switch (mCurrentState) {
    case EECurrentBucketsState::NewBucketFilesDownloading:
        qDebug() << "Files list for downloading received!";
        mDataLoader->downloadingFilesListReceived();
        break;
    case EECurrentBucketsState::FileStatusCheck:
        qDebug() << "File list for checking received!";
        mDataTreeBuilder->getFilesInfoForNextBucket();
        break;
    case EECurrentBucketsState::ExistingBucketFilesUploading:
    case EECurrentBucketsState::NewBucketsFileUploading:
    case EECurrentBucketsState::FileUpdating:
        qDebug() << "Syncronizate date time for file!";
        mDateTimeSyncronizator->synchronizateFileDateTime();
        break;
    default:
        qDebug() << "Unknown file state";
        break;
    }
}

/**
 * @brief handleContinueNextProcess
 * After date time for file has been syncronizated - call previous process
 */
void EEDataManager::handleContinueNextProcess()
{
    qDebug() << "- handleContinueNextProcess()";

    switch (mCurrentState) {
    case EECurrentBucketsState::ExistingBucketFilesUploading:
        qDebug() << "Files for existing bucket uploading...";
        mDataLoader->uploadNextFiles();
        break;
    case EECurrentBucketsState::NewBucketsFileUploading:
         qDebug() << "New Buckets File Uploading buckets list";
    case EECurrentBucketsState::FileUpdating:
         qDebug() << "Keep updating...";
         mDataLoader->uploadBucketData();
         break;
    case EECurrentBucketsState::NewBucketFilesDownloading:
         qDebug() << "New bucket files downloading...";
         mDataLoader->startDataDownloading();
        break;
    case EECurrentBucketsState::ExistingBucketFilesDownloading:
        qDebug() << "Files for existing bucket downloading...";
        mDataLoader->cloudFilesDownloading();
        break;
    default:
        qDebug() << "Unknown previous state state";
        break;
    }
}
