#include "eedatamanager.h"
#include "eefolderparsecontroller.h"
#include "eefoldermodel.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "eestatustimer.h"

#include "EEContainers/eebucket.h"

#include "eesdk.h"
#include "EESDK/eefileloader.h"
#include "eedatatreebuilder.h"
#include "eedatetimesyncronizator.h"
#include "eedatacomparator.h"
#include "eedataloader.h"

#include "crypto.h"
#include "controller.h"

#include <QFile>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>



EEDataManager::EEDataManager(EESDK *sdk, EEFileLoader *loader, QString path, QObject *parent) :
    QObject(parent),
    mSdk{sdk},
    mLoader{loader},
    mFolderParseController{new EEFolderParseController},
    mBucketFacade{new EEBucketFacade},
    mDataTreeBuilder{new EEDataTreeBuilder(mSdk, mBucketFacade, this)},
    mDateTimeSyncronizator{new EEDateTimeSyncronizator(mSdk,mBucketFacade, mFolderParseController, this)},
    mDataComparator{new EEDataComparator(mSdk, mBucketFacade, mFolderParseController, this)},
    mDataLoader{new EEDataLoader(mSdk, mBucketFacade,mFolderParseController, mLoader, this)}
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->setWorkingDirectory(path);

    connect(mSdk, SIGNAL(bucketsReceived(QList<EEBucket*>)), this, SLOT(handleBucketsListReceive(QList<EEBucket*>)));
    connect(mSdk, SIGNAL(filesForBucketReceived(QList<EEFile>)), this, SLOT(handleFilesInfoReceived(QList<EEFile>)));
    connect(mLoader, SIGNAL(fileUploaded()), this, SLOT(handleFilesUploaded()));
    connect(mLoader, SIGNAL(fileDownloaded()), this, SLOT(handleFilesDownloading()));

    connect(&EEStatusTimer::getInstance(),&EEStatusTimer::checkStatusByTimer,
            mDataTreeBuilder, [this] () {
        setCurrentState(EECurrentBucketsState::FileStatusCheck);
        mDataTreeBuilder->startBucketsStatusCheck();
    });

    connect(mDataTreeBuilder, SIGNAL(cloudTreeReceived()), this, SLOT(handleCloudTreeReceived()));

    connect(mDataComparator, SIGNAL(dataCompared()), this, SLOT(startDataUpdate()));

    connect(mDataLoader, &EEDataLoader::startFilesUpload, this, [this]() {
        qDebug() << "All neccesary files removed! Start to upload...";
        setCurrentState(EECurrentBucketsState::ExistingBucketFilesUploading);
        //start to upload data for setted before upload list
        mDataLoader->uploadNextFiles();
    });

    connect(mDataLoader, &EEDataLoader::uploadingFinished, this, [this]() {
        qDebug() << "Uploading finished! Start to download files...";
        setCurrentState(EECurrentBucketsState::NewBucketFilesDownloading);
        mDataLoader->useNextBucketForDownloading();
    });

    connect(mDataLoader, &EEDataLoader::newBucketsDownloaded, this, [this]() {
        qDebug() << "All new buckets files were donwloaded! Start to donwload files for existing...";

        setCurrentState(EECurrentBucketsState::ExistingBucketFilesDownloading);
        mDataLoader->cloudFilesDownloading();
    });

    connect(mDataLoader, &EEDataLoader::startDateSyncronization, this, [this]() {
        qDebug() << "Start date syncronization...";
        setCurrentState(EECurrentBucketsState::FileDateSyncronization);
        mDateTimeSyncronizator->initializeDateSyncronization();
    });

    connect(mSdk, SIGNAL(bucketDeleted(QString)), mDataLoader, SLOT(startUpdateNextBucket()));
    connect(mSdk, SIGNAL(bucketCreated(EEBucket*)), mDataLoader, SLOT(bucketCreated(EEBucket*)));
    connect(mLoader, SIGNAL(fileDeleted()), mDataLoader, SLOT(cloudFilesUpdating()));
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
void EEDataManager::initializeDeletionProcess()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    setCurrentState(EECurrentBucketsState::FileDeletion);

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
 * if local data has even been initialized before - download all the data from cloud
 * if cloud data hasn't been initalized yet or been initialized just part of data - delete it and upload all of the local
 * In others cases - start to compare all of the folders and files
 */
void EEDataManager::checkDataStatus()
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
            qDebug() <<" LOCAL FOLDER IS NOT EMPTY AND EXISTS - START TO COMPARE DATA!!!";
            mDataComparator->startCompareData();
        } else {
            qDebug() << "Local directory is empty!!! Start to donwload all the data from cloud...";
            initLocalFromCloud();
        }

    } else {
        //if cloud data hasn't been initalized or initalized wrong way
        qDebug() << "No information about root folder on the cloud! Try to upload it first";
        initializeDeletionProcess();
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
 * @brief EEDataManager::handleCloudTreeReceived
 * Calls after data about buckets' files were received
 */
void EEDataManager::handleCloudTreeReceived()
{
    if (mCurrentState == EECurrentBucketsState::FileStatusCheck) {
        qDebug() << "Start check data status!";
        checkDataStatus();
    } else if (mCurrentState == EECurrentBucketsState::FileDateSyncronization) {
        qDebug() << "Start syncronizate data";
        mDateTimeSyncronizator->syncronizeLocalDataWithCloud();
    } else {
        qDebug() << "Unknown state!";
    }
}

/**
 * @brief EEDataManager::handleFilesDownloading
 * Calls after succesful file download
 */
void EEDataManager::handleFilesDownloading()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    switch (mCurrentState) {
    case EECurrentBucketsState::ExistingBucketFilesDownloading:
        qDebug() << "Files for existing bucket uploading...";
        mDataLoader->cloudFilesDownloading();
        break;
    case EECurrentBucketsState::NewBucketFilesDownloading:
        qDebug() << "New bucket files downloading buckets list";
    case EECurrentBucketsState::FileUpdating:
        qDebug() << "Keep updating...";
        mDataLoader->startDataDownloading();
        break;
    default:
        qDebug() << "Unknown file donwloading state!";
        break;
    }
}
/**
 * @brief EEDataManager::handleFilesUploaded
 * Calls after file upload
 */
void EEDataManager::handleFilesUploaded()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

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
    default:
        qDebug() << "Unknown file uploading state!";
        break;
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
    } else if (mCurrentState == EECurrentBucketsState::FileDateSyncronization) {
        qDebug() << "Date updating buckets list";
        mDataTreeBuilder->getFilesInfoForNextBucket();
    } else if (mCurrentState == EECurrentBucketsState::FileDeletion){
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
void EEDataManager::handleFilesInfoReceived(QList<EEFile> files)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    for (auto &file : files) {
        QString lFileName = file.filename();
        EEBucket* lFileBucket = mBucketFacade->bucketById(file.bucketId());
        if (lFileBucket == nullptr) {
            qDebug() << "Error! Bucket is not initialized!!!" << file.bucketId();
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
            qDebug() << "File name has been succesfully decrypted!";
            lFileName = QString::fromUtf8(result);
        } else {
            qDebug() << "File name couldn't be decrypted!";
            lFileName = "Unknown_Name";
        }
        file.setFilename(lFileName);
    }

    if (mCurrentState == EECurrentBucketsState::NewBucketFilesDownloading) {
        qDebug() << "Files list for downloading received!";
        mDataLoader->filesListReceived(files);
    } else if (mCurrentState == EECurrentBucketsState::FileStatusCheck) {
        qDebug() << "File list for checking received!";
        mDataTreeBuilder->setFilesInformationForBucket(files);
    } else if (mCurrentState == EECurrentBucketsState::FileDateSyncronization) {
        qDebug() << "File date syncronization";
        mDataTreeBuilder->setFilesInformationForBucket(files);
    } else {
        qDebug() << "Unknown file state";
    }
}
