#include "eedataloader.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "EEDataSync/EELocalDataParser/eejsonbuilder.h"
#include "EEDataSync/EELocalDataParser/eefolderparsecontroller.h"
#include "eefoldermodel.h"


#include "EESDK/EEContainers/eebucket.h"
#include "EESDK/EEContainers/eefile.h"
#include "EESDK/eefileloader.h"
#include "eestatustimer.h"
#include "eesettingsclass.h"

#include "EESDK/eesdk.h"

#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QPointer>
#include <typeinfo>

EEDataLoader::EEDataLoader(EESDK *sdk,
                           EEJsonBuilder *builder,
                           EEBucketFacade *facade,
                           EEFolderParseController *folderParseController,
                           EEFileLoader *loader,
                           QObject *parent)
    : QObject(parent),
      mSdk{sdk},
      mJsonBuilder{builder},
      mBucketFacade{facade},
      mFolderParseController{folderParseController},
      mLoader{loader}
{

}
/**
 * @brief EEDataLoader::deleteNextBucket
 * Delete bucket according to deletion queue.
 * After buckets deletion start to delete files
 */
void EEDataLoader::deleteNextBucket()
{
    qDebug() << "- EEDataLoader::deleteNextBucket()";

    EEBucket *lBucket = mBucketFacade->nextBucketToRemove();
    if (lBucket != nullptr) {
        mSdk->destroyBucket(lBucket->id());
    } else {
        qDebug() << "All required buckets have been deleted!";
        qDebug() << "Start to delete files!";
        deleteNextFile();
    }

}
/**
 * @brief EEDataLoader::deleteNextFile
 * Delete file from bucket according to files deletion queue
 * After deletion emit signal about whole data complited deletion
 */
void EEDataLoader::deleteNextFile()
{
    if (mBucketFacade->setCurrentFileByDeletionQueue()) {
        EEFile *lFile = mBucketFacade->currentDeletionFile();
        mLoader->deleteFile(lFile->bucketId(), lFile->id());
    } else {
        qDebug() << "All required data has been deleted!";
        qDebug() << "Start to compare data actuality";
        emit dataDeleted();
    }
}

/**
 * @brief EEDataLoader::createBucket
 * Create bucket according to current bucket in usage
 */
void EEDataLoader::createBucket()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    qDebug() << "Folder name: " << mBucketFacade->currentUploadingFolderName();
    //create bucket for folder
    QString lBucketName = mBucketFacade->currentUploadingFolderName();

    //if folder's status cannot be updated, because it hasn't been added to json yet - add it
    if (!mJsonBuilder->updateStatus(OperationType::Upload , lBucketName)) {
        EEFolderParseController *lCurrentFolderParser = new EEFolderParseController(this);
        lCurrentFolderParser->startSubfoldersInitialization(mBucketFacade->workingDirectory() + lBucketName);
        mJsonBuilder->addCurrentObjectToJson(QDateTime(), lCurrentFolderParser->rootModel());
        mJsonBuilder->updateCurrentElementStatus();
    }

    mSdk->createBucket(lBucketName);
}


/**
 * @brief EEDataLoader::uploadFile
 * Upload file to currently using bucket
 */
void EEDataLoader::uploadFile()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    QString lFilePath = mFolderParseController->filePath(mBucketFacade->currentUploadingFileNameFromFolderModel());
    QString lBucketId = mBucketFacade->currentBucket()->id();
    qDebug() << "File name to upload : " << lFilePath;
    qDebug() << "Bucket id: " << lBucketId;
    QString lFileName = lFilePath.split('/').takeLast();

    mBucketFacade->setCurrentFileName(lFileName);

    //if file's status cannot be updated, because it hasn't been added to json yet - add it
    if (!mJsonBuilder->updateStatus( OperationType::Upload, mBucketFacade->currentBucket()->name(), lFileName)) {
        mJsonBuilder->addCurrentObjectToJson();
    }
    mLoader->uploadData(lBucketId, lFilePath);
}

/**
 * @brief EEDataLoader::downloadFile
 * Download file into restore library
 */
void EEDataLoader::downloadFile()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEBucket* lCurrentBucket = mBucketFacade->currentBucket();
    QString lFileId = mBucketFacade->temporaryBucketCurrentFile()->id();
    QString lFolderPath = lCurrentBucket->name();

    QString lSeparator = "/";
    if (!lFolderPath.isEmpty()) {
        if (lFolderPath.at(0) != lSeparator) {
            lFolderPath = lSeparator + lFolderPath;
        }
    }

    QString lFileName = mBucketFacade->temporaryBucketCurrentFile()->filename();
    mBucketFacade->setCurrentFileName(lFileName);
    QString lPath = mBucketFacade->workingDirectory() + lFolderPath + lSeparator + lFileName;


    //if file's status cannot be updated, because it hasn't been added to json yet - add it
    if (!mJsonBuilder->updateStatus(OperationType::Download, lFolderPath, lFileName)) {
        mJsonBuilder->addCurrentObjectToJson();
    }

    qDebug() << "Path to save : " << lPath;
    mLoader->downloadFile(lCurrentBucket->id(), lFileId, lPath);
}

/**
 * @brief EEDataLoader::filesListReceived
 * This method starts to using after receiving list of file
 * for current bucket and decryption of their names.
 * Call another method to start process of files downloading
 * @param files - list of files of backet
 */
void EEDataLoader::downloadingFilesListReceived()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->setTemporaryDownloadingQueueFiles(mBucketFacade->filesByBucketId(mBucketFacade->currentBucket()->id()));
    mBucketFacade->setCurrentFileIndex(-1);
    startDataDownloading();
}

/**
 * @brief EEDataLoader::startDataDownloading
 * Download files for current bucket
 * or if all files have already been downloaded
 * move to the next bucket
 */
void EEDataLoader::startDataDownloading()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    int lNumberOfFiles = mBucketFacade->temporaryBucketFilesListSize();
    if (mBucketFacade->currentFileIndex() + 1 < lNumberOfFiles) {
        qDebug() << "Move to the next file";
        //move to the next file
        mBucketFacade->increaseCurrentIndex();
        downloadFile();
    } else {
        qDebug() << "Move to the next bucket";
        useNextBucketForDownloading();
    }
}

/**
 * @brief EEDataLoader::bucketsListReceived
 * Slot, which activate after receiving list of buckets.
 * Initialize list of buckets and queue of buckets to download
 * Start process of downloading
 * @param buckets - list of buckets
 */
void EEDataLoader::bucketsListReceived()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->initializeDownloadingQueue();
    useNextBucketForDownloading();
}
/**
 * @brief EEDataLoader::startDataUpdate
 * Start process of uploding/downloading/updating data if needed
 */
void EEDataLoader::startDataUpdate()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (!mBucketFacade->updateBucketQueue().isEmpty()) {
        qDebug() << "Update bucket queue...";
        mBucketFacade->initializeUploadingBucketsQueueFromUpdateQueue();
        updateNextBucket();
        return;
    } else if (!mBucketFacade->updatingFilesQueue().isEmpty()){
        qDebug() << "Update files queue...";
        mBucketFacade->setUpdateFilesQueueToUploadQueue();
        cloudFilesUpdating();
        return;
    } else if(!mBucketFacade->uploadingFilesQueue().isEmpty()) {
       qDebug() << "Uploading files queue...";
       emit startFilesUploading();
       return;
    } else if (!mBucketFacade->downloadingBucketsQueue().isEmpty()) {
        qDebug() << "Download buckets...";
        emit uploadingFinished();
        return;
    } else if (!mBucketFacade->downloadingFileQueue().isEmpty()) {
        qDebug() << "Downloading files...";
        emit startFilesDownloading();
        return;
    } else {
        qDebug() << "Everything up-to-date";
        EEStatusTimer::getInstance().startTimer();
    }
}


/**
 * @brief EEDataLoader::updateNextBucket
 * Delete neccesary buckets and then upload them from local machine
 */
void EEDataLoader::updateNextBucket()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    //search buckets by name and delete by id
    if (mBucketFacade->setCurrentFolderModelByUpdatingBucket()) {
        QString lFolderName = mBucketFacade->currentUpdatingFolderName();
        EEBucket* lCheckBucket = mBucketFacade->bucketByName(lFolderName);

        if (lCheckBucket != nullptr) {
            //if folder's status cannot be updated, because it hasn't been added to json yet - add it
            if (!mJsonBuilder->updateStatus(OperationType::Update, lCheckBucket->name())) {
                EEFolderParseController *lCurrentFolderParser = new  EEFolderParseController(this);
                lCurrentFolderParser->startSubfoldersInitialization(mBucketFacade->workingDirectory() + lCheckBucket->name());
                mJsonBuilder->addCurrentObjectToJson(QDateTime(), lCurrentFolderParser->rootModel());
                mJsonBuilder->updateCurrentElementStatus();

            }

            mSdk->destroyBucket(lCheckBucket->id());

            if (mBucketFacade->removeBucketByBucketId(lCheckBucket->id())) {
                qDebug() << "Bucket removed from local list!";
            } else {
                qDebug() << "Bucket wasn't removed from list of local buckets!";
            }
        } else {
            qDebug() << "Bucket is new and haven't been initialized to destruction!";
            qDebug() << "Try to create it...";
            updateNextBucket();
        }
    } else {
        qDebug() << "All neccesary buckets removed! Start to upload...";
        //start to upload data for setted before upload list
        createNextBucket();
    }
}

/**
 * @brief EEDataLoader::startUseNextBucket
 * Move to the next bucket. Reset file index and send request for bucket creation.
 * After all buckets were created - start to
 */
void EEDataLoader::createNextBucket()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (mBucketFacade->setCurrentFolderModelByUploadingBucket()) {
        mBucketFacade->setCurrentFileIndex(-1);
        createBucket();
    } else {
        qDebug() << "All new buckets were uploaded!";
        qDebug() << "Start to upload files into the old ones...";
        mBucketFacade->setUpdateFilesQueueToUploadQueue();

        cloudFilesUpdating();
    }
}


/**
 * @brief EEDataLoader::bucketCreated
 * Receive signal about bucket creation
 * and start to upload its data(files)
 * @param newbucket
 */
void EEDataLoader::bucketCreated(EEBucket *newbucket)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mJsonBuilder->updateCurrentElementStatus();

    mBucketFacade->setCurrentBucket(newbucket);
    mBucketFacade->addBucketToAllBuckets(newbucket);

    uploadBucketData();
}


/**
 * @brief EEDataLoader::startDataUploading
 * Control uploading of the files into bucket
 * and moving to the next one, of needed
 */
void EEDataLoader::uploadBucketData()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    int lNumberOfFiles = mBucketFacade->currentUploadingFolderSize();
    if (mBucketFacade->currentFileIndex() + 1 < lNumberOfFiles) {
        qDebug() << "Move to the next file";
        //move to the next file
        mBucketFacade->increaseCurrentIndex();
        uploadFile();
    } else {
        qDebug() << "Move to the next bucket";
        createNextBucket();
    }
}

/**
 * @brief EEDataLoader::cloudFilesUpdating
 * Delete outdated files from cloud.
 * After deletion - start to upload new on theirs places
 */
void EEDataLoader::cloudFilesUpdating()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    //search buckets by name and delete by id
    if (mBucketFacade->setCurrentFileByUpdatingQueue()) {
        EEFile *lCheckFile = mBucketFacade->currentUpdatingFile();
        if (lCheckFile != nullptr) {
            mLoader->deleteFile(lCheckFile->bucketId(), lCheckFile->id());
        } else {
            qDebug() << "Bucket is new and haven't been initialized to destruction!";
            qDebug() << "Try to move forward...";
            cloudFilesUpdating();
        }
    } else {
        qDebug() << "All cloud files are deleted!";
        emit startFilesUploading();
    }
}


/**
 * @brief EEDataLoader::uploadNextFiles
 * Upload files for existing buckets.
 * After uploading all of the buckets - start process of download required
 */
void EEDataLoader::uploadNextFiles()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (mBucketFacade->setCurrentFileByUploadingQueue()) {
        EEFile *lCheckFile = mBucketFacade->currentUploadingFile();
        mBucketFacade->setCurrentFileName(lCheckFile->filename());

        QString lWorkingDirectory = mBucketFacade->workingDirectory();

        if (lWorkingDirectory.isEmpty()) {
            qDebug() << "Folder is not initalized!!!";
        }

        EEBucket *lCheckBucket = mBucketFacade->bucketById(lCheckFile->bucketId());
        mBucketFacade->setCurrentBucket(lCheckBucket);

        if (lCheckBucket != nullptr) {
            QString lBucketName = lCheckBucket->name();
            QString lSeparator = "/";

            if (!lBucketName.isEmpty()) {
                if (lBucketName.at(0) != lSeparator) {
                    lBucketName = lSeparator + lBucketName;
                }
            }

            //if file's status cannot be updated, because it hasn't been added to json yet - add it
            if (!mJsonBuilder->updateStatus(OperationType::Upload, lBucketName, lCheckFile->filename())) {
                mJsonBuilder->addCurrentObjectToJson();
            }

            mLoader->uploadData(lCheckFile->bucketId(),
                                lWorkingDirectory + lBucketName + lSeparator + lCheckFile->filename());
        } else {
            qDebug() << "Error! Bucket for uploading wasn't found!" << lCheckFile->bucketId();
        }
    } else {
        qDebug() << "Files uploading finished!";
        emit uploadingFinished();
    }
}

/**
 * @brief EEDataLoader::useNextBucketForDownloading
 * Initialize process of files donwloading for setted bucket.
 * After all files for created buckets were donwloaded - start to download files for existing buckets
 */
void EEDataLoader::useNextBucketForDownloading()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (mBucketFacade->setCurrentBucketByDownloadingQueue()) {
        if (!mJsonBuilder->updateStatus(OperationType::Download, mBucketFacade->currentBucket()->name())) {
            //use qpoiinter to avoid memory leaks
            QPointer<EEFolderModel>lFolderModel = new EEFolderModel;
            lFolderModel->setName(mBucketFacade->currentBucket()->name());;
            mJsonBuilder->addCurrentObjectToJson(QDateTime(), lFolderModel);
        }
        getFilesListForCurrentBucket();
    } else {
        qDebug() << "Start to download files from existing folders...";
        emit startFilesDownloading();
    }
}


/**
 * @brief EEDataLoader::getFilesListForCurrentBucket
 * Create directory for current bucket, if needed
 * Send get request for this bucket files
 */
void EEDataLoader::getFilesListForCurrentBucket()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    //create folder, if needed
    if (mBucketFacade->currentBucket() != nullptr) {
        QString lBucketName = mBucketFacade->currentBucket()->name();
        QString lSeparator = "/";

        if (!lBucketName.isEmpty()) {
            if (lBucketName.at(0) != lSeparator) {
                lBucketName = lSeparator + lBucketName;
            }
        }

        QString lPath = QString(mBucketFacade->workingDirectory() + lBucketName );
        QDir lCurrentDirectory(lPath);
        if (!lCurrentDirectory.exists()) {
            lCurrentDirectory.mkpath(".");
        }

        mSdk->getFilesFromBucket(mBucketFacade->currentBucket()->id());
    } else {
        qDebug() << "There no current bucket facade";
    }
}


/**
 * @brief EEDataLoader::cloudFilesDownloading
 * Download files for existing bucket.
 * After start process of date syncronizatin
 */
void EEDataLoader::cloudFilesDownloading()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (mBucketFacade->setCurrentFileByDownloadingQueue()) {
        EEFile *lCheckFile = mBucketFacade->currentDownloadingFile();
        EEBucket* lCurrentBucket = mBucketFacade->bucketById(lCheckFile->bucketId());

        if (lCurrentBucket!= nullptr) {
            if (lCheckFile != nullptr) {
                mBucketFacade->setCurrentBucket(lCurrentBucket);
                mBucketFacade->setCurrentFileName(lCheckFile->filename());

                QString lBucketName = lCurrentBucket->name();
                QString lSeparator = "/";

                if (!lBucketName.isEmpty()) {
                    if (lBucketName.at(0) != lSeparator) {
                        lBucketName = lSeparator + lBucketName;
                    }
                }

                QString lPath = mBucketFacade->workingDirectory() + lBucketName ;

                //if file's status cannot be updated, because it hasn't been added to json yet - add it
                if (!mJsonBuilder->updateStatus(OperationType::Download, lBucketName, lCheckFile->filename())) {
                    mJsonBuilder->addCurrentObjectToJson();
                }

                mLoader->downloadFile(lCheckFile->bucketId(), lCheckFile->id(),
                                      lPath + lSeparator + lCheckFile->filename());
            } else {
                qDebug() << "Error! Bucket for downloading wasn't found!" << lCheckFile->bucketId();
            }
        } else {
            qDebug() << "No bucket with such id! File cannot be downloaded..." << lCheckFile->bucketId();
            qDebug() << "Try to use next file for download...";
            cloudFilesDownloading();
        }
    } else {
        qDebug() << "All data has been downloaded/uploaded!";
        EEStatusTimer::getInstance().startTimer();
    }
}
