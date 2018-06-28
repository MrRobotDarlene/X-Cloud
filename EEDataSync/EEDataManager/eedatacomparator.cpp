#include "eedatacomparator.h"
#include "EESDK/EEContainers/eebucket.h"
#include "EESDK/EEContainers/eefile.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "EEDataSync/EELocalDataParser/eefolderparsecontroller.h"

#include <QDebug>
#include <typeinfo>


EEDataComparator::EEDataComparator(EESDK *sdk, EEBucketFacade *facade, EEFolderParseController *folderParseController, QObject *parent)
    : QObject(parent),
      mSdk{sdk},
      mBucketFacade{facade},
      mFolderParseController{folderParseController}
{

}

/**
 * @brief EEDataComparator::initializeComparationLocalDataWithJson
 * Initialize process of comparation local data with json data
 * @param jsonFolderModel
 */
void EEDataComparator::initializeOutdatedDataComparation(EEFolderModel *jsonFolderModel, EEFolderModel *cloudFolderModel)
{
    qDebug() << typeid(*this).name() << __FUNCTION__;
    EEFolderModel *lRoot = mFolderParseController->rootModel();
    EEFolderModel *lJsonRoot = jsonFolderModel;
    EEFolderModel *lCloudRoot = cloudFolderModel;

    if (lRoot != nullptr) {
        if (lJsonRoot != nullptr) {
            if (lCloudRoot != nullptr) {
                if (lRoot->name() == lJsonRoot->name()) {
                    if (lCloudRoot->name() == lJsonRoot->name()) {
                        //check root folder's files
                        compareLocalFilesWithJson(lRoot, lJsonRoot, lCloudRoot);

                        //move through its subfolders
                        compareLocalFoldersWithJson(lRoot, lJsonRoot, lCloudRoot);
                    } else {
                        qDebug() << "Cloud doesn't contain root!";
                    }
                } else {
                    qDebug() << "Json or local files doesn't contain root folder";
                }
            } else {
                qDebug() << "Cloud root folder is not initialized";
            }
        } else {
            qDebug() << "Json root folder is not initalized";
        }
    } else {
        qDebug() << "Local root folder is not initialized!";
    }

}
/**
 * @brief EEDataComparator::compareLocalFoldersWithJson
 * Compare local folders with json.
 * If exists in both - start to compare files too
 * IF exists in json and not locally - add to deletion list
 * @param localFolder - root local folder
 * @param jsonFolder - root json folder
 */
void EEDataComparator::compareLocalFoldersWithJson(EEFolderModel *localFolder, EEFolderModel *jsonFolder, EEFolderModel *cloudFolder)
{
    foreach (EEFolderModel *subJsonFolder, jsonFolder->folderList()) {
        bool lIsLocalExists = false;
        EEFolderModel *lTemporaryFolder = nullptr;
        foreach (EEFolderModel *subLocalFolder, localFolder->folderList()) {
            if (subLocalFolder->name() == subJsonFolder->name()) {
                lTemporaryFolder = subLocalFolder;
                lIsLocalExists = true;
                break;
            }
        }

        if (!lIsLocalExists) {
            qDebug() << "Folder has been removed local and should be removed from cloud: "
                     << subJsonFolder->name();
            //add also all of the subfolders
            addFolderWithSubfoldersToDeletionList(subJsonFolder);
        } else {
            EEFolderModel *lCloudTemporaryFolder = nullptr;
            if (cloudFolder != nullptr) {
                bool lIsCloudExists = false;
                foreach (EEFolderModel *subCloudFolder, cloudFolder->folderList()) {
                    if (subCloudFolder->name() == lTemporaryFolder->name()) {
                        lCloudTemporaryFolder = subCloudFolder;
                        lIsCloudExists = true;
                        break;
                    }
                }


                if (!lIsCloudExists) {
                    qDebug() <<"Add local folder to deletion list" << lTemporaryFolder->name();
                    //add to local deletion
                    mBucketFacade->addOutdateFolderToRemoveList(lTemporaryFolder);
                }
            }

            //first check files for this directory
            compareLocalFilesWithJson( lTemporaryFolder, subJsonFolder, lCloudTemporaryFolder);

            //keep move through its subfolders
            compareLocalFoldersWithJson(lTemporaryFolder, subJsonFolder, lCloudTemporaryFolder);
        }
    }
}
/**
 * @brief EEDataComparator::compareLocalFilesWithJson
 * If file is not exists locally anymore - add to deletion files list
 * @param localFolder - local folder, which files have to be compared
 * @param jsonFolder - same folder as it described in json
 */
void EEDataComparator::compareLocalFilesWithJson(EEFolderModel *localFolder, EEFolderModel *jsonFolder, EEFolderModel *cloudFolder)
{
    foreach (EEModel *jsonFile, jsonFolder->filesList() ) {
        bool lIsLocalExists = false;
        foreach (EEModel *localFile, localFolder->filesList()) {
            if (localFile->name() == jsonFile->name()) {
                lIsLocalExists = true;
                break;
            }
        }

        if (!lIsLocalExists) {
            qDebug() << "Try to add file to deletion list " << jsonFile->name();
            EEBucket *lBucket =  mBucketFacade->bucketByName(jsonFolder->name());
            if (lBucket != nullptr) {
                EEFile *lFile = mBucketFacade->bucketFileByName(jsonFile->name().split("/").takeLast(), lBucket->id());
                if (lFile != nullptr) {
                    qDebug() << "Added succesfully";
                    mBucketFacade->addToDeletionFilesList(lFile);
                } else {
                    qDebug() << "No such file: " << jsonFile->name();
                }
            } else {
                qDebug() << "No such bucket: " << jsonFolder->name();
            }
        } else {
            //search for local files to delete
            if (cloudFolder != nullptr) {
                bool lIsCloudExists = false;
                foreach (EEModel *cloudFile, cloudFolder->filesList()) {
                    if (cloudFile->name() == jsonFile->name()) {
                        lIsCloudExists = true;
                        break;
                    }
                }

                if (!lIsCloudExists) {
                    qDebug() << "Add file to local deletion list" << jsonFile->name();
                    //add file to local deletion files
                    mBucketFacade->addOutdatedLocalFile(jsonFile->name());
                }
            }
        }
    }
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
 * @brief EEDataComparator::addFolderWithSubfoldersToDeletionList
 * Recursively add bucket and its subbuckets to deletion list
 * @param model
 */
void EEDataComparator::addFolderWithSubfoldersToDeletionList(EEFolderModel *model)
{
    EEBucket *lBucket = mBucketFacade->bucketByName(model->name());

    if (lBucket != nullptr) {
        mBucketFacade->addToDeletionBucketQueue(lBucket);
    } else {
        qDebug() << "Bucket has already been removed from cloud! " << model->name();
    }

    for (auto folder: model->folderList()) {
        addFolderWithSubfoldersToDeletionList(folder);
    }
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
    foreach (EEFolderModel *folder, parentFolder->folderList()) {
        EEBucket* lCheckBucket = mBucketFacade->bucketByName(folder->name());
        if (lCheckBucket != nullptr) {
            if (mBucketFacade->removeBucketFromDownloadingQueue(lCheckBucket)) {
                qDebug() << "Removed bucket from downloading queue! " << lCheckBucket->name();
            } else {
                qDebug() << " Bucket cannot be removed! Maybe it has already been removed!" << lCheckBucket->name();
            }
            buildUpdateFilesForFolder(folder->filesList(),
                                      mBucketFacade->filesByBucketId(lCheckBucket->id()),
                                      lCheckBucket->name());
            qDebug() << "Bucket is already on the cloud! Move forward " << lCheckBucket->name();
        } else {
            qDebug() << "Bucket is not on the cloud yet. Add it! " << folder->name();
            mBucketFacade->addElementToUpdateBucketQueue(folder);
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
void EEDataComparator::buildUpdateFilesForFolder(QList<EEModel*> localFiles, QList<EEFile*> cloudFiles, QString bucketName)
{
    mBucketFacade->setTemporaryDownloadingQueueFiles(cloudFiles);

    QString lSeparator = "/";
    foreach (EEModel *localFile, localFiles) {
        //add to upload queue, if local file is not in the list of cloud files
        if (!listHasName(cloudFiles, localFile->name().split(lSeparator).takeLast())) {
            mBucketFacade->addModelToUploadFilesQueue(localFile->name().split(lSeparator).takeLast(),
                                                          mBucketFacade->bucketByName(bucketName)->id());
        } else {
            foreach (EEFile* cloudFile, cloudFiles) {
                //special case for root
                if (bucketName == QString("/")) {
                    bucketName = "";
                } else {
                    if (!bucketName.isEmpty())  {
                        if (bucketName.at(0) != lSeparator) {
                            bucketName = lSeparator + bucketName;
                        }
                    }
                }
                if (localFile->name() == (bucketName + lSeparator + cloudFile->filename())) {
                    //if local file is newer or same datetime as file on cloud
                    //remove it from downloading files list
                    if (localFile->updated().secsTo(cloudFile->created()) <= 0) {
                        qDebug() << "Time difference:" << localFile->updated().secsTo(cloudFile->created());
                        if (mBucketFacade->removeFileFromTemporaryDownloadingQueue(cloudFile)) {
                            qDebug() << "File removed from downloading list:" << cloudFile->filename();
                        } else {
                            qDebug() << "File cannot be removed from downloading list:" << cloudFile->filename();
                        }
                        //if local file is later - add to update list
                        if (localFile->updated().secsTo(cloudFile->created()) < 0) {
                            mBucketFacade->addFileToUpdateFileQueue(cloudFile);
                            //break;
                        }
                    } else {
                        qDebug() << "Local file is older! Keep it in downloading files queue..." << cloudFile->filename();
                    }
                } else if (!listHasName(localFiles, bucketName + lSeparator + cloudFile->filename())) {
                    qDebug() << "Keep file in the downloading queue : " << cloudFile->filename();
                    //break;
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
bool EEDataComparator::listHasName(QList<EEFile*> list, QString name)
{
    foreach (EEFile *file, list) {
        if (file->filename() == name) {
            return true;
        }
    }
    return false;
}
