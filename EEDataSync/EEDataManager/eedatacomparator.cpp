#include "eedatacomparator.h"
#include "EEContainers/eebucket.h"
#include "EEContainers/eefile.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "EEDataSync/EEParser/eefolderparsecontroller.h"

#include <QDebug>


EEDataComparator::EEDataComparator(EESDK *sdk, EEBucketFacade *facade, EEFolderParseController *folderParseController, QObject *parent)
    : QObject(parent),
      mSdk{sdk},
      mBucketFacade{facade},
      mFolderParseController{folderParseController}
{

}
/**
 * @brief EEDataComparator::startCompareData
 * Start process of data comparation.
 * First, set all of the buckets to downloading buckets queue.
 * Later, if this bucket(folder) exists locally, it will be removed from the queue
 * Also, build files for updating list for root folder
 * and call recurcively buiding of list for other folders and theirs files
 */
void EEDataComparator::startCompareData()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    QList<EEBucket*>lRemoteBucketsList = mBucketFacade->allBuckets();
//    if (mFolderParseController == nullptr) {
//        mFolderParseController = new EEFolderParseController;
//    }


//    mFolderParseController->startSubfoldersInitialization(mBucketFacade->workingDirectory());

    EEFolderModel *lRoot = mFolderParseController->rootModel();
    //first, initialize donwloading queue with all buckets
    //if it doesn't need to be downloaded - remove it from list
    mBucketFacade->setDownloadingBucketQueue(lRemoteBucketsList);

    //first of all, compare root bucket files
    mBucketFacade->removeBucketFromDownloadingQueue(mBucketFacade->rootBucket());
    buildUpdateFilesForFolder(lRoot->filesList(),
                              mBucketFacade->filesByBucketId(mBucketFacade->rootBucket()->id()),
                              mBucketFacade->rootBucket()->name());

    //then start to compare recurcively
    buildUpdateFolders(lRoot);


    qDebug() << "Data has been compared!!!";
    emit dataCompared();
}


/**
 * @brief EEDataComparator::buildUpdateFolders
 * Build list of buckets, which have to be updated
 * If bucket doesn't exist in cloud buckets list - add it
 * If exists - remove from downloading queue and start to compare all of the files
 * @param parentFolder
 */
void EEDataComparator::buildUpdateFolders(EEFolderModel * const parentFolder)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    foreach (EEFolderModel *folder, parentFolder->folderList()) {
        EEBucket* lCheckBucket = mBucketFacade->bucketByName(folder->name());
        if (lCheckBucket != nullptr) {
            qDebug() << lCheckBucket->name();
            if (mBucketFacade->removeBucketFromDownloadingQueue(lCheckBucket)) {
                qDebug() << "Removed bucket from downloading queue! " << lCheckBucket->name();
            }
#warning should be changed to pointer!
            buildUpdateFilesForFolder(folder->filesList(),
                                      mBucketFacade->filesByBucketId(lCheckBucket->id()),
                                      lCheckBucket->name());
            qDebug() << "Bucket is already on the cloud! Move forward " << lCheckBucket->name();
        } else {
            //add bucket, which is not on the cloud yet
            qDebug() << "Bucket is not on the cloud yet. Add it! " << folder->name();
            mBucketFacade->addElementToUpdateBucketQueue(folder);
            //create bucket!;
        }
        buildUpdateFolders(folder);
    }

}
/**
 * @brief EEDataComparator::buildUpdateFilesForFolder
 * Check files list of local and cloud buckets.
 * Compare them and add to the downloading or uploading list
 * depends on result of comparation.
 * First, all of the files added to temporary downloading list
 * If one of them doens't need to be downloaded - remove from this list
 * and start to check does it need to be updated. If has to be updated, add to update files list.
 * After cycles are finished, add left files from temporary down. list to regular
 * @param localFiles - list of local files
 * @param cloudFiles - list of cloud files
 * @param bucketName
 */
void EEDataComparator::buildUpdateFilesForFolder(QList<EEModel*> localFiles, QList<EEFile> cloudFiles, QString bucketName)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->setOneBucketDownloadingQueueFiles(cloudFiles);
    QString lSeparator = "/";
#ifdef WIN32
    lSeparator = "\\";
#endif
    foreach (EEModel *localFile, localFiles) {
        //add to upload queue, if local file is not in the list of cloud files
        if (!listHasName(cloudFiles, localFile->name().split(lSeparator).takeLast())) {
            qDebug() << localFile->name().replace(bucketName, "");
            mBucketFacade->addModelToUploadFilesQueue(localFile->name().split(lSeparator).takeLast(),
                                                      mBucketFacade->bucketByName(bucketName)->id());
        } else {
            foreach (EEFile cloudFile, cloudFiles) {
                //special case for root
                if (bucketName == "/") {
                    bucketName = "";
                } else {
                    if (!bucketName.isEmpty())  {
                        if (bucketName.at(0) != lSeparator) {
                            bucketName = lSeparator + bucketName;
                        }
                    }
                }
                if (localFile->name() == (bucketName + lSeparator + cloudFile.filename())) {
                    //if local file is newer or same datetime as file on cloud
                    //remove it from downloading files list
                    if (localFile->updated().secsTo(cloudFile.created()) <= 0) {
                        qDebug() << "Time difference:" << localFile->updated().secsTo(cloudFile.created());
                        if (mBucketFacade->removeFileFromOneBucketDownloadingQueue(cloudFile)) {
                            qDebug() << "File removed from downloading list:" << cloudFile.filename();
                        } else {
                            qDebug() << "File cannot be removed from donwloading list:" << cloudFile.filename();
                        }
                        //if local file is later - add to update list
                        if (localFile->updated().secsTo(cloudFile.created()) < 0) {
                            mBucketFacade->addFileToUpdateFileQueue(cloudFile);
                            break;
                        }
                    } else {
                        qDebug() << "Local file is older! Keep it in downloading files queue..." << cloudFile.filename();
                        qDebug() << "Time difference:" << localFile->updated().secsTo(cloudFile.created());
                    }
                } else if (!listHasName(localFiles, bucketName + lSeparator + cloudFile.filename())) {
                    qDebug() << "Keep file in the downloading queue : " << cloudFile.filename();
                    break;
                }
            }
        }
   }

    //set all left files to list of files, which have to be downloaded
    mBucketFacade->addCurrentFilesToDownloadQueue();
}

/**
 * @brief listHasName
 * Compare cloud file with all files in the folder on the local machine
 * @param list - list of local files
 * @param name - cloud file name
 * @return
 */
bool EEDataComparator::listHasName(QList<EEModel *> list, QString name)
{
    foreach (EEModel *model, list) {
        if (model->name() == name) {
            return true;
        }
    }
    return false;
}
/**
 * @brief EEDataComparator::listHasName
 * Compare local file with all files in the folder(bucket) on the cloud
 * @param list - list of cloud files
 * @param name- local file name
 * @return
 */
bool EEDataComparator::listHasName(QList<EEFile> list, QString name)
{
    foreach (EEFile file, list) {
        if (file.filename() == name) {
            return true;
        }
    }
    return false;
}
