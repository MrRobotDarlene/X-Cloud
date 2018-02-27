#ifndef EEAPI_H
#define EEAPI_H

#include "eesdk.h"
#include "eecallbackcontroller.h"

#include <QObject>

typedef enum {
    EEFileLoaderStateUpload,
    EEFileLoaderStateDownload
} EEFileLoaderState;

class EEFileLoader;

extern EEFileLoader *gCryptoPointer;

class EEFileLoader : public QObject
{
    Q_OBJECT
public:
    explicit EEFileLoader(EESDK *sdk, QObject *parent = nullptr);

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


    void handleCallbackResult(CallbackResult callResult);


private:
    EEShardRequest makeShards();
signals:
    void fileUploaded();
    void fileDownloaded();
    void moveToTheNextFile();

private slots:
    void frameCreated(EEFrame);
    void shardAddedToFrame(EEShard);
    void shardsReceived(QList<EEShard>);

    void requestError(EEError, QString method);

private:
    EESDK *mSdk;
    EEFileLoaderState mState;
    QString mBucketId;
    QString mFileId;
    QString mFileName;
    QByteArray mData;
    EEFrame mFrame;
};

#endif // EEAPI_H
