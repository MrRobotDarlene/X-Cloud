#include "eedataloader.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "EEDataSync/EEParser/eefolderparsecontroller.h"

#include "EEContainers/eebucket.h"
#include "EESDK/eefileloader.h"
#include "eestatustimer.h"
#include "eesettingsclass.h"

#include "eesdk.h"

#include <QDebug>
#include <QDir>

EEDataLoader::EEDataLoader(EESDK *sdk, EEBucketFacade *facade,  EEFolderParseController *folderParseController, EEFileLoader *loader, QObject *parent)
    : QObject(parent),
      mSdk{sdk},
      mBucketFacade{facade},
      mFolderParseController{folderParseController},
      mLoader{loader}
{

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

    mSdk->createBucket(lBucketName);
}


/**
 * @brief EEDataLoader::uploadFile
 * Upload file to currently using bucket
 */
void EEDataLoader::uploadFile()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    QString lFileName = mFolderParseController->filePath(mBucketFacade->currentUploadingFileNameFromFolderModel());
    QString lBucketId = mBucketFacade->currentBucket()->id();
    qDebug() << "File name to upload : " << lFileName;
    qDebug() << "Bucket id: " << lBucketId;

    mLoader->uploadData(lBucketId, lFileName);
}

/**
 * @brief EEDataLoader::downloadFile
 * Download file into restore library
 */
void EEDataLoader::downloadFile()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEBucket* lCurrentBucket = mBucketFacade->currentBucket();
    QString lFileId = mBucketFacade->currentFile().id();
    QString lFolderPath = lCurrentBucket->name();

    QString lSeparator = "/";
#ifdef WIN32
    lSeparator = "\\";
#endif
    if (!lFolderPath.isEmpty()) {
        if (lFolderPath.at(0) != lSeparator) {
            lFolderPath = lSeparator + lFolderPath;
        }
    }

    QString lFileName = mBucketFacade->currentFile().filename();
    QString lPath = mBucketFacade->workingDirectory() + lFolderPath + lSeparator + lFileName;

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
void EEDataLoader::filesListReceived(QList<EEFile> files)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->setOneBucketDownloadingQueueFiles(files);
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

    int lNumberOfFiles = mBucketFacade->currentFilesListSize();
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

#warning maybe return should be removed!
    if (!mBucketFacade->updateBucketQueue().isEmpty()) {
        qDebug() << "Update bucket queue...";
        mBucketFacade->initializeUploadingBucketsQueueFromUpdateQueue();
        startUpdateNextBucket();
        return;
    } else if (!mBucketFacade->updatingFilesQueue().isEmpty()){
        qDebug() << "Update files queue...";
        cloudFilesUpdating();
        return;
    } else if(!mBucketFacade->uploadingFilesQueue().isEmpty()) {
       qDebug() << "Uploading files queue...";
       emit startFilesUpload();
       return;
    } else if (!mBucketFacade->downloadingBucketsQueue().isEmpty()) {
        qDebug() << "Download buckets...";
        emit uploadingFinished();
        return;
    } else if (!mBucketFacade->downloadingFileQueue().isEmpty()) {
        qDebug() << "Downloading files...";
        emit newBucketsDownloaded();
        return;
    } else {
        qDebug() << "Everything up-to-date";
        EEStatusTimer::getInstance().startTimer();
    }
}


/**
 * @brief EEDataLoader::startUpdateNextBucket
 * Delete neccesary buckets and then upload them from local machine
 */
void EEDataLoader::startUpdateNextBucket()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    //search buckets by name and delete by id
    if (mBucketFacade->setCurrentFolderModelByUpdatingBucket()) {
        QString lFolderName = mBucketFacade->currentUpdatingFolderName();
        EEBucket* lCheckBucket = mBucketFacade->bucketByName(lFolderName);
        if (lCheckBucket != nullptr) {
            mSdk->destroyBucket(lCheckBucket->id());

            if (mBucketFacade->removeBucketByBucketId(lCheckBucket->id())) {
                qDebug() << "Bucket removed from local list!";
            } else {
                qDebug() << "Bucekt wasn't removed from list of local buckets!";
            }
        } else {
            qDebug() << "Bucket is new and haven't been initialized to destruction!";
            qDebug() << "Try to create it...";
            startUpdateNextBucket();
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

    mBucketFacade->setUpdateFilesQueueToUploadQueue();
    //search buckets by name and delete by id
    if (mBucketFacade->setCurrentFileByUpdatingQueue()) {
        //QString lFolderName = mBucketFacade->currentUploadingFile();
        EEFile lCheckFile = mBucketFacade->currentUpdatingFile();
        if (!lCheckFile.id().isEmpty()) {
            mLoader->deleteFile(lCheckFile.bucketId(), lCheckFile.id());
        } else {
            qDebug() << "Bucket is new and haven't been initialized to destruction!";
            qDebug() << "Try to move forward...";
            cloudFilesUpdating();
        }
    } else {
        qDebug() << "All cloud files are deleted!";
        emit startFilesUpload();
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
        EEFile lCheckFile = mBucketFacade->currentUploadingFile();

        QString lFolderName = EESettingsClass::getInstance().getSettingsValue(SettingsOptions::FolderName).toString();

        if (lFolderName.isEmpty()) {
            qDebug() << "Folder is not initalized!!!";
        }

        EEBucket *lCheckBucket = mBucketFacade->bucketById(lCheckFile.bucketId());
        if (lCheckBucket != nullptr) {
            QString lBucketName = lCheckBucket->name();

            QString lSeparator = "/";
        #ifdef WIN32
            lSeparator = "\\";
        #endif

            if (!lBucketName.isEmpty()) {
                if (lBucketName.at(0) != lSeparator) {
                    lBucketName = lSeparator + lBucketName;
                }
            }

            mLoader->uploadData(lCheckFile.bucketId(),
                                lFolderName + lBucketName + lSeparator + lCheckFile.filename());
        } else {
            qDebug() << "Error! Bucket for uploading wasn't found!" << lCheckFile.bucketId();
        }


    } else {
        qDebug() << "Emit uploading finished...";
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
        getFilesListForCurrentBucket();
    } else {
        qDebug() << "Start to download files from existing folders...";
        emit newBucketsDownloaded();
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
    #ifdef WIN32
        lSeparator = "\\";
    #endif

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
        EEFile lCheckFile = mBucketFacade->currentDownloadingFile();
        EEBucket* lCurrentBucket = mBucketFacade->bucketById(lCheckFile.bucketId());

        if (lCurrentBucket!= nullptr) {
            if (!lCheckFile.id().isEmpty()) {
                QString lBucketName = lCurrentBucket->name();
                QString lSeparator = "/";
            #ifdef WIN32
                lSeparator = "\\";
            #endif

                if (!lBucketName.isEmpty()) {
                    if (lBucketName.at(0) != lSeparator) {
                        lBucketName = lSeparator + lBucketName;
                    }
                }

                QString lPath = mBucketFacade->workingDirectory() + lBucketName + lSeparator + lCheckFile.filename();
                mLoader->downloadFile(lCheckFile.bucketId(), lCheckFile.id(), lPath);
            } else {
                qDebug() << "Error! Bucket for downloading wasn't found!" << lCheckFile.bucketId();
            }
        } else {
            qDebug() << "No bucket with such id! File cannot be downloaded..." << lCheckFile.bucketId();
            qDebug() << "Try to use next file for download...";
            cloudFilesDownloading();
        }
    } else {
        qDebug() << "All data has been downloaded/uploaded!";
        qDebug() << "Start to syncronizate dates...";
        emit startDateSyncronization();
    }
}
