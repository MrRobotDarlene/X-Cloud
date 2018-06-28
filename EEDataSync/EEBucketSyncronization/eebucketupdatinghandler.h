#ifndef EEBUCKETUPDATINGHANDLER_H
#define EEBUCKETUPDATINGHANDLER_H

#include "eebucketbasehandler.h"
#include "eefoldermodelholder.h"


class EEFolderModel;
class EEBucket;
class EEFolderModelHolder;

/**
 * @brief The EEBucketUpdatingHandler class
 * Class, which are in charge of data upading.
 */
class EEBucketUpdatingHandler :  public EEBucketBaseHandler, public EEFolderModelHolder
{
public:
    EEBucketUpdatingHandler(EEBucketDataHolder *bucketData);
    ~EEBucketUpdatingHandler();

    QString currentFileNameFromFolderModel() const;

    EEBucket *rootBucket() const;
    EEBucket *lastBucket() const;
};

#endif // EEBUCKETUPDATINGHANDLER_H
