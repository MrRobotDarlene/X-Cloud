#ifndef EEAPI_H
#define EEAPI_H

#include "eesdk.h"
#include "eecallbackcontroller.h"

#include <QObject>

class EEFileLoader;
/**
 * @brief gCryptoPointer
 * Variable to handle callbacks from c code
 */
extern EEFileLoader *gCryptoPointer;

/**
 * @brief The EEFileLoader class
 * Class for uploading, downloading and deltiong data
 * Also, handle its responces
 */
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
    void uploadData(const QString &bucketId, const QString &name);

    /**
     * @brief EEFileLoader::downloadFile
     * Download file by its bucket id and file id
     * using .c method for it
     * @param bucketId
     * @param fileId
     */
    void downloadFile(const QString &bucketId, const QString &fileId, const QString &fileName);

    /**
     * @brief EEFileLoader::deleteFile
     * Delete file from cloud usign bucketid and its file
     * @param bucketId
     * @param fileId
     */
    void deleteFile(const QString &bucketId, const QString &fileId);
    /**
     * @brief EEFileLoader::handleCallbackResult
     * Method, which handle callback about download/upload from .c program
     * @param callResult
     */
    void handleCallbackResult(CallbackResult callResult);


signals:
    void fileUploaded();
    void fileUploadingError();
    void fileDownloaded();
    void fileDowloadingError();
    void fileDeleted();
    void fileDeletionError();

private:
    EESDK *mSdk;
};

#endif // EEAPI_H
