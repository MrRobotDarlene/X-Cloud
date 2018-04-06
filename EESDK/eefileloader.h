#ifndef EEAPI_H
#define EEAPI_H

#include "eesdk.h"
#include "eecallbackcontroller.h"

#include <QObject>

class EEFileLoader;

extern EEFileLoader *gCryptoPointer;

class EEFileLoader : public QObject
{
    Q_OBJECT
public:
    explicit EEFileLoader(EESDK *sdk, QObject *parent = nullptr);
    ~EEFileLoader();

    /**
     * @brief EEFileLoader::uploadData
     * Upload file using its name and bucket id
     * using .c method for it
     * @param bucketId - id of bucket
     * @param name - path to file on the local machine
     */
    void uploadData(QString bucketId, QString name);

    /**
     * @brief EEFileLoader::downloadFile
     * Download file by its bucket id and file id
     * using .c method for it
     * @param bucketId
     * @param fileId
     */
    void downloadFile(QString bucketId, QString fileId, QString fileName);

    /**
     * @brief EEFileLoader::deleteFile
     * Delete file from cloud usign bucketid and its file
     * @param bucketId
     * @param fileId
     */
    void deleteFile(QString bucketId, QString fileId);
    /**
     * @brief EEFileLoader::handleCallbackResult
     * Method, which handle callback about download/upload from .c program
     * @param callResult
     */
    void handleCallbackResult(CallbackResult callResult);


signals:
    void fileUploaded();
    void fileDownloaded();
    void fileDeleted();

private:
    EESDK *mSdk;
};

#endif // EEAPI_H
