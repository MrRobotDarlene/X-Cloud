#include "eefileloader.h"
#include "eesdkparser.h"
#include "../EEDataSync/EEParser/eefolderparsecontroller.h"
#include "controller.h"

#include <QCryptographicHash>
#include <QDebug>

#define SIZE_BYTES(v) v*8.0

EEFileLoader *gCryptoPointer;


EEFileLoader::EEFileLoader(EESDK *sdk, QObject *parent) : QObject(parent),
    mSdk{sdk}
{
    gCryptoPointer = this;

    connect(mSdk, SIGNAL(shardsReceived(QList<EEShard>)), this, SLOT(shardsReceived(QList<EEShard>)));
    connect(mSdk, SIGNAL(frameCreated(EEFrame)), this, SLOT(frameCreated(EEFrame)));
    connect(mSdk, SIGNAL(shardAddedToFrame(EEShard)), this, SLOT(shardAddedToFrame(EEShard)));

    connect(mSdk, SIGNAL(requestError(EEError, QString)), this, SLOT(requestError(EEError, QString)));
}

/**
 * @brief EEFileLoader::uploadData
 * Upload file using its name and bucket id
 * using .c method for it
 * @param bucketId - id of bucket
 * @param name - path to file on the local machine
 */
void EEFileLoader::uploadData(QString bucketId, QString name)
{
    mBucketId = bucketId;
    mFileName = name;
    mState = EEFileLoaderStateUpload;

    //required for correct casting
    QByteArray lBucketIdByteArray = bucketId.toLatin1();
    QByteArray lFileNameByteArray = name.toLatin1();

    //list of arguments
    char *lBucketCharId = lBucketIdByteArray.data();
    char *lFilePath = lFileNameByteArray.data();

    if (start_upload_file(lBucketCharId, lFilePath)) {
        qDebug() << "Unsuccesful file uploading! Move to the next file!";

        emit moveToTheNextFile();
    }
}

/**
 * @brief EEFileLoader::downloadFile
 * Download file by its bucket id and file id
 * using .c method for it
 * @param bucketId
 * @param fileId
 * @param fileName
 */
void EEFileLoader::downloadFile(QString bucketId, QString fileId, QString fileName)
{
    mState = EEFileLoaderStateDownload;
    mFileId = fileId;
    mBucketId = bucketId;

    //required for correct casting
    QByteArray lBucketIdByteArray = bucketId.toLatin1();
    QByteArray lFileIdByteArray = fileId.toLatin1();
    QByteArray lFileName = fileName.toLatin1();

    //list of arguments
    char *lBucketCharId = lBucketIdByteArray.data();
    char *lFileCharId = lFileIdByteArray.data();
    char *lFilePath = lFileName.data();

    start_download_file(lBucketCharId, lFileCharId, lFilePath);
}

void EEFileLoader::handleCallbackResult(CallbackResult callResult)
{
    switch (callResult) {
    case ErrorCallBack:
        qDebug() << "Error! Unsuccesful downloading/uploading!";
        emit fileUploaded();
        break;
    case FileDownloaded:
        emit fileDownloaded();
        break;
    case FileUploaded:
        emit fileUploaded();
        break;
    default:
        qDebug() << "Unknown result!";
        break;
    }
    qDebug() << "END HANDLING RESULT!";
}

EEShardRequest EEFileLoader::makeShards()
{

}

void EEFileLoader::frameCreated(EEFrame frame)
{
    qDebug() << __FUNCTION__;
    mFrame = frame;
    switch (mState) {
    case EEFileLoaderStateDownload:

        break;
    case EEFileLoaderStateUpload:
        mSdk->createToken(mBucketId, mFileName, EEToken::OperationPush);
        break;
    default:
        break;
    }
}

void EEFileLoader::shardAddedToFrame(EEShard shard)
{
    qDebug() << __FUNCTION__;
    mSdk->uploadShardToNode(shard, mData);
}

void EEFileLoader::shardsReceived(QList<EEShard> list)
{
    qDebug() << __FUNCTION__;

    foreach (EEShard shard, list) {
        mSdk->downloadShardFromNode(shard);
    }
}

void EEFileLoader::requestError(EEError, QString method)
{
    qDebug() << __FUNCTION__;

}
