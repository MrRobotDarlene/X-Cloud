#include "eebucketupdatinghandler.h"
#include "EEDataSync/EELocalDataParser/eefoldermodel.h"
#include "eebucketdataholder.h"
#include "EESDK/EEContainers/eebucket.h"

#include <QDebug>

EEBucketUpdatingHandler::EEBucketUpdatingHandler(EEBucketDataHolder *bucketData):
    EEBucketBaseHandler{bucketData},
    EEFolderModelHolder{}
{
}

EEBucketUpdatingHandler::~EEBucketUpdatingHandler()
{

}


QString EEBucketUpdatingHandler::currentFileNameFromFolderModel() const
{
    return currentFolderModel()->filesList().at(bucketData()->currentFileIndex())->name();
}


EEBucket* EEBucketUpdatingHandler::rootBucket() const
{
    EEBucket *lRoot = nullptr;
    auto lBuckets = bucketData()->allBuckets();
    for (auto bucket : lBuckets) {
        if (bucket->name() == QString('/')) {
            lRoot = bucket;
            break;
        }
    }
    return lRoot;
}

EEBucket* EEBucketUpdatingHandler::lastBucket() const
{
    auto lBuckets = bucketData()->allBuckets();
    EEBucket * lLastBucket = nullptr;

    if (!lBuckets.isEmpty()) {
        lLastBucket = lBuckets.at(0);
        for (auto bucket : lBuckets) {
            if (bucket->created() > lLastBucket->created()) {
                lLastBucket = bucket;
            }
        }
    }

    return lLastBucket;
}
