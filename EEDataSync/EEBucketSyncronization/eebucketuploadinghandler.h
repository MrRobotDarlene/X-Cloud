#ifndef EEBUCKETUPLOADINGHANDLER_H
#define EEBUCKETUPLOADINGHANDLER_H

#include "eebucketdataholder.h"

#include <QQueue>

class EEFolderModel;
class EEBucket;
/**
 * @brief The EEBucketUploadingHandler class
 * Class, which are in charge of buckets uploading.
 * Holds queue of buckets to upload and upload them one by one
 */
class EEBucketUploadingHandler : public EEBucketDataHolder
{
public:
    EEBucketUploadingHandler(EEBucketDataHolder *bucketData);
    ~EEBucketUploadingHandler();

    void addElementToUploadingBucketQueue(EEFolderModel *newFolder);
    QQueue<EEFolderModel *> uploadingBucketsQueue() const;
    void setUploadingBucketQueue(QQueue<EEFolderModel *> queue);

    /**
     * @brief EEBucketUploadingHandler::setCurrentFolderModelByUploadingBucket
     * Upload buckets starting from last child to root.
     * In this case root folder will have last time of creation and it's better for syncronization of the data
     * @return
     */
    bool setCurrentFolderModelByUploadingBucket();
    EEFolderModel *currentFolderModel() const;
    QString currentFolderName() const;
    int currentFolderSize() const;
    QString currentFileNameFromFolderModel() const;


    QQueue<EEFile>uploadingFilesQueue() const;
    void addFileToUploaingFileQueue(EEFile newFile);
    bool setCurrentFileByUploadingQueue();
    void addModelToUploadFilesQueue(QString fileName, QString bucketId);


    void addQueueToUploadingFileQueue(QQueue<EEFile> fileQueue);

    EEFile currentUploadingFile();


protected:
    EEBucketDataHolder *mBucketData;
private:
    QQueue<EEFolderModel*> mUploadingBucketQueue;
    EEFolderModel *mCurrentFolderModel;

    QQueue<EEFile> mUploadingFilesQueue;
    EEFile mCurrentFile;
};

#endif // EEBUCKETUPLOADINGHANDLER_H
