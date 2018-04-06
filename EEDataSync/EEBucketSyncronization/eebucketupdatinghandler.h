#ifndef EEBUCKETUPDATINGHANDLER_H
#define EEBUCKETUPDATINGHANDLER_H

#include "eebucketuploadinghandler.h"
#include "EEContainers/eefile.h"

/**
 * @brief The EEBucketUpdatingHandler class
 * Class, which are in charge of data upading.
 * Most of methods are inherited from uploading,
 * In this case queue of folders this is queue of the elements,
 * which have to be deleted and uploaded again
 */
class EEBucketUpdatingHandler : public EEBucketUploadingHandler//,public EEBUcketDownloadingHandler
{
public:
    EEBucketUpdatingHandler(EEBucketDataHolder *bucketData);

    QQueue<EEFolderModel *> updatingBucketQueue() const;
    void addElementToUpdateBucketQueue(EEFolderModel *folder);

    QQueue<EEFile>updatingFilesQueue() const;
    void addFileToUpdateFileQueue(EEFile file);
    bool setCurrentFileByUpdatingQueue();

    EEFile currentUpdatingFile();

    bool setCurrentFolderModelByUpdatingBucket();
    EEBucket *rootBucket() const;
    EEBucket *lastBucket() const;
};

#endif // EEBUCKETUPDATINGHANDLER_H
