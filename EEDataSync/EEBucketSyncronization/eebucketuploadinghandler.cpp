#include "eebucketuploadinghandler.h"
#include "eefoldermodel.h"

#include <QQueue>
#include <QDebug>

EEBucketUploadingHandler::EEBucketUploadingHandler(EEBucketDataHolder *bucketData):
    mBucketData{bucketData},
    mCurrentFolderModel{nullptr}
{
}

EEBucketUploadingHandler::~EEBucketUploadingHandler()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    if (!mUploadingBucketQueue.isEmpty()) {
        qDeleteAll(mUploadingBucketQueue);
    }
}

void EEBucketUploadingHandler::addElementToUploadingBucketQueue(EEFolderModel *newFolder)
{
    bool lIsAlreadyExists = false;
    foreach (EEFolderModel *folder, mUploadingBucketQueue) {
        if (*folder == *newFolder) {
            qDebug() << "Folder is already on the list";
            lIsAlreadyExists = true;
            break;
        }
    }

    if (!lIsAlreadyExists) {
        qDebug() << "Adding new bucket to the list of buckets!";
        mUploadingBucketQueue.append(newFolder);
    }
}

QQueue<EEFolderModel *> EEBucketUploadingHandler::uploadingBucketsQueue() const
{
    return mUploadingBucketQueue;
}

void EEBucketUploadingHandler::setUploadingBucketQueue(QQueue<EEFolderModel *> queue)
{
    if (!mUploadingBucketQueue.isEmpty()) {
        qDeleteAll(mUploadingBucketQueue);
        mUploadingBucketQueue.clear();
    }

    mUploadingBucketQueue = queue;
}
/**
 * @brief EEBucketUploadingHandler::setCurrentFolderModelByUploadingBucket
 * Upload buckets starting from last child to root.
 * In this case root folder will have last time of creation and it's better for syncronization of the data
 * @return
 */
bool EEBucketUploadingHandler::setCurrentFolderModelByUploadingBucket()
{
    bool lIsBucketSetted = false;
    if (!mUploadingBucketQueue.isEmpty()) {
        //mCurrentFolderModel = mUploadingBucketQueue.dequeue();
        mCurrentFolderModel = mUploadingBucketQueue.takeLast();
        lIsBucketSetted = true;
    }
    return lIsBucketSetted;
}

EEFolderModel *EEBucketUploadingHandler::currentFolderModel() const
{
    return mCurrentFolderModel;
}

QString EEBucketUploadingHandler::currentFolderName() const
{
    return mCurrentFolderModel->name();
}

int EEBucketUploadingHandler::currentFolderSize() const
{
    return mCurrentFolderModel->filesList().size();
}

QString EEBucketUploadingHandler::currentFileNameFromFolderModel() const
{
    return mCurrentFolderModel->filesList().at(mBucketData->currentFileIndex())->name();
}

QQueue<EEFile> EEBucketUploadingHandler::uploadingFilesQueue() const
{
    return mUploadingFilesQueue;
}

void EEBucketUploadingHandler::addFileToUploaingFileQueue(EEFile newFile)
{
    bool lIsAlreadyExists = false;
    foreach (EEFile file, mUploadingFilesQueue) {
        if (file == newFile) {
            qDebug() << "File is already on the list";
            lIsAlreadyExists = true;
            break;
        }
    }

    if (!lIsAlreadyExists) {
        qDebug() << "Adding new file to the list of files!";
        mUploadingFilesQueue.append(newFile);
    }
}

bool EEBucketUploadingHandler::setCurrentFileByUploadingQueue()
{
    bool lIsBucketSetted = false;
    if (!mUploadingFilesQueue.isEmpty()) {
        mCurrentFile = mUploadingFilesQueue.dequeue();
        lIsBucketSetted = true;
    }
    return lIsBucketSetted;
}

void EEBucketUploadingHandler::addModelToUploadFilesQueue(QString fileName, QString bucketId)
{
    EEFile lNewFile;
    lNewFile.setFilename(fileName);
    lNewFile.setBucketId(bucketId);

    bool lIsAlreadyExists = false;
    foreach (EEFile file, mUploadingFilesQueue) {
        if (file.filename() == lNewFile.filename()) {
            if (file.bucketId() == lNewFile.bucketId()) {
                qDebug() << "File is already on the list : " << lNewFile.filename();
                lIsAlreadyExists = true;
                break;
            }
        }
    }

    if (!lIsAlreadyExists) {
        qDebug() << "Adding new file to the list of files!" << lNewFile.filename();;
        mUploadingFilesQueue.append(lNewFile);
    }
}

void EEBucketUploadingHandler::addQueueToUploadingFileQueue(QQueue<EEFile>fileQueue)
{
    bool lIsAlreadyExists = false;

    foreach (EEFile newFile, fileQueue) {
        lIsAlreadyExists = false;
        foreach (EEFile file, mUploadingFilesQueue) {
            if (file.filename() == newFile.filename()) {
                if (file.bucketId() == newFile.bucketId()) {
                    qDebug() << "File is already on the list : " << newFile.filename();
                    lIsAlreadyExists = true;
                    break;
                }
            }
        }

        if (!lIsAlreadyExists) {
            qDebug() << "Adding new file to the list of files! " << newFile.filename();
            mUploadingFilesQueue.append(newFile);
        }
    }
}

EEFile EEBucketUploadingHandler::currentUploadingFile()
{
    return mCurrentFile;
}
