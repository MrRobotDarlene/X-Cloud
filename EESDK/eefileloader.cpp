#include "eefileloader.h"
#include "eesdkparser.h"
#include "../EEDataSync/EEParser/eefolderparsecontroller.h"
#include "controller.h"

#include <QDebug>

EEFileLoader *gCryptoPointer;


EEFileLoader::EEFileLoader(EESDK *sdk, QObject *parent) : QObject(parent),
    mSdk{sdk}
{
    gCryptoPointer = this;
}

EEFileLoader::~EEFileLoader()
{
    cleanVariables();
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
//    name = "\""+ name + "\"";

    //required for correct casting
    QByteArray lBucketIdByteArray = bucketId.toLatin1();
    QByteArray lFileNameByteArray = name.toLatin1();

    //list of arguments
    char *lBucketCharId = lBucketIdByteArray.data();
    char *lFilePath = lFileNameByteArray.data();

    qDebug() << "File to upload path:" << lFilePath;
    if (start_upload_file(lBucketCharId,
                          lFilePath,
                          mSdk->getEmail().toUtf8().data(),
                          mSdk->getPassword().toUtf8().data(),
                          mSdk->getBridge().toUtf8().data())) {
        qDebug() << "Unsuccesful file uploading! Move to the next file!";
        emit fileUploaded();
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
//    fileName = "\""+ fileName + "\"";

    //required for correct casting
    QByteArray lBucketIdByteArray = bucketId.toLatin1();
    QByteArray lFileIdByteArray = fileId.toLatin1();
    QByteArray lFileName = fileName.toLatin1();

    //list of arguments
    char *lBucketCharId = lBucketIdByteArray.data();
    char *lFileCharId = lFileIdByteArray.data();
    char *lFilePath = lFileName.data();

    qDebug() << "File to download path:" << lFilePath;
    if (start_download_file(lBucketCharId,
                        lFileCharId,
                        lFilePath,
                        mSdk->getEmail().toUtf8().data(),
                        mSdk->getPassword().toUtf8().data(),
                        mSdk->getBridge().toUtf8().data())) {
        qDebug() << "Unsuccesful file downloading! Move to the next file...";
        emit fileDownloaded();
    }
}
/**
 * @brief EEFileLoader::deleteFile
 * Delete file from cloud usign bucketid and its file
 * @param bucketId
 * @param fileId
 */
void EEFileLoader::deleteFile(QString bucketId, QString fileId)
{

    QByteArray lBucketIdByteArray = bucketId.toLatin1();
    QByteArray lFileIdByteArray = fileId.toLatin1();

    //list of arguments
    char *lBucketCharId = lBucketIdByteArray.data();
    char *lFileCharId = lFileIdByteArray.data();

    if (start_delete_file(lBucketCharId, lFileCharId,
                        mSdk->getEmail().toUtf8().data(),
                        mSdk->getPassword().toUtf8().data(),
                        mSdk->getBridge().toUtf8().data())) {
        qDebug() << "Unsuccesful file deletion!";
        emit fileDeleted();
    }

}

/**
 * @brief EEFileLoader::handleCallbackResult
 * Method, which handle callback about download/upload from .c program
 * @param callResult
 */
void EEFileLoader::handleCallbackResult(CallbackResult callResult)
{
    switch (callResult) {
    case ErrorDownloadingCallBack:
        qDebug() << "Error! Unsuccesful downloading!";
    case FileDownloaded:
        emit fileDownloaded();
        break;
    case ErrorUploadingCallback:
        qDebug() << "Error! Unsuccesful uploading!";
    case FileUploaded:
        emit fileUploaded();
        break;
    case ErrorFileDeletion:
        qDebug() << "Unsuccesful file deletion!";
        break;
    case FileDeleted:
        emit fileDeleted();
        break;
    default:
        qDebug() << "Unknown result!";
        break;
    }
}
