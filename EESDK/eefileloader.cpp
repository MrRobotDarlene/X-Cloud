#include "eefileloader.h"
#include "eesdkparser.h"
#include "eefolderparsecontroller.h"
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
void EEFileLoader::uploadData(const QString &bucketId, const QString &name)
{
    //required for correct casting
    QByteArray lBucketIdByteArray = bucketId.toLatin1();
    QByteArray lFileNameByteArray = name.toLatin1();

    //list of arguments
    char *lBucketCharId = lBucketIdByteArray.data();
    char *lFilePath = lFileNameByteArray.data();

    qDebug() << "File to upload path:" << lFilePath;

    //char *bridge = (char*)"https://api.internxt.com:6382";
    //char* lPhoneNumber = (char*)"+380970892691";

    if (start_upload_file(lBucketCharId,
                          lFilePath,
                          mSdk->getLogin().toUtf8().data(),
                          mSdk->getPassword().toUtf8().data(),
                          mSdk->getBridge().toUtf8().data())) {
        qDebug() << "Unsuccesful file uploading! Move to the next file!";
        emit fileUploadingError();
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
void EEFileLoader::downloadFile(const QString &bucketId, const QString &fileId, const QString &fileName)
{
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
                        mSdk->getLogin().toUtf8().data(),
                        mSdk->getPassword().toUtf8().data(),
                        mSdk->getBridge().toUtf8().data())) {
        qDebug() << "Unsuccesful file downloading! Move to the next file...";
        emit fileDowloadingError();
    }
}
/**
 * @brief EEFileLoader::deleteFile
 * Delete file from cloud usign bucketid and its file
 * @param bucketId
 * @param fileId
 */
void EEFileLoader::deleteFile(const QString &bucketId, const QString &fileId)
{
    QByteArray lBucketIdByteArray = bucketId.toLatin1();
    QByteArray lFileIdByteArray = fileId.toLatin1();

    //list of arguments
    char *lBucketCharId = lBucketIdByteArray.data();
    char *lFileCharId = lFileIdByteArray.data();

    if (start_delete_file(lBucketCharId, lFileCharId,
                        mSdk->getLogin().toUtf8().data(),
                        mSdk->getPassword().toUtf8().data(),
                        mSdk->getBridge().toUtf8().data())) {
        qDebug() << "Unsuccesful file deletion!";
        emit fileDeletionError();
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
        emit fileDowloadingError();
        break;
    case FileDownloaded:
        qDebug() << "Emit file downloaded!";
        emit fileDownloaded();
        break;
    case ErrorUploadingCallback:
        qDebug() << "Error! Unsuccesful uploading!";
        emit fileUploadingError();
        break;
    case FileUploaded:
        qDebug() << "Emit file uploaded!";
        emit fileUploaded();
        break;
    case ErrorFileDeletion:
        qDebug() << "Unsuccesful file deletion!";
        emit fileDeletionError();
        break;
    case FileDeleted:
        emit fileDeleted();
        break;
    default:
        qDebug() << "Unknown result!";
        break;
    }
}
