#include "eedatatreebuilder.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "EESDK/eesdk.h"
#include "eestatustimer.h"

#include <QDebug>

#include <typeinfo>

EEDataTreeBuilder::EEDataTreeBuilder(EESDK *sdk, EEBucketFacade *facade, QObject *parent) :
    QObject(parent),
    mSdk{sdk},
    mBucketFacade{facade}
{

}

/**
 * @brief EEDataManager::startBucketsStatusCheck
 * Start to check folder status.
 * Activates after timer timeout.
 */
void EEDataTreeBuilder::startBucketsStatusCheck()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEStatusTimer::getInstance().stopCheck();
    mSdk->getBuckets();
}

/**
 * @brief EEDataManager::getFilesInfoForNextBucket
 * download files by id for bucket!
 * After download - start to check data statuses
 */
void EEDataTreeBuilder::getFilesInfoForNextBucket()
{
    //use all buckets to get files from each of them
    if (mBucketFacade->setNextCurrentBucket()) {
        QString lBucketId = mBucketFacade->currentBucket()->id();
        mSdk->getFilesFromBucket(lBucketId);
    } else {
        qDebug() << "All information about files for bucket have been downloaded!";
        emit cloudTreeReceived();
    }
}

