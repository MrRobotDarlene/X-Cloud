#ifndef EEBUCKETUPLOADINGHANDLER_H
#define EEBUCKETUPLOADINGHANDLER_H

#include "eebucketbasehandler.h"
#include "eefoldermodelholder.h"


#include <QQueue>

class EEFolderModel;
class EEBucket;
class EEFolderModelHolder;

/**
 * @brief The EEBucketUploadingHandler class
 * Class, which are in charge of buckets uploading.
 * Holds queue of buckets to upload and upload them one by one
 */
class EEBucketUploadingHandler : public EEBucketBaseHandler, public EEFolderModelHolder
{
public:
    EEBucketUploadingHandler(EEBucketDataHolder *bucketData);
    ~EEBucketUploadingHandler();

    QString currentFileNameFromFolderModel() const;
    void addModelToFilesQueue(QString fileName, QString bucketId);
};

#endif // EEBUCKETUPLOADINGHANDLER_H
