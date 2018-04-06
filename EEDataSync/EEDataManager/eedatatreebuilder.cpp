#include "eedatatreebuilder.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "EESDK/eesdk.h"
#include "eestatustimer.h"

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
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    //use all buckets to get files from each of them
    if (mBucketFacade->setNextCurrentBucket()) {
        QString lBucketId = mBucketFacade->currentBucket()->id();
        mSdk->getFilesFromBucket(lBucketId);
    } else {
        qDebug() << "All information about files for bucket have been downloaded!";

        emit cloudTreeReceived();
    }
}

/**
 * @brief EEDataManager::saveBuckets
 * Save list of buckets from cloud and call method for data status check
 * @param buckets
 */
void EEDataTreeBuilder::setFilesInformationForBucket(QList<EEFile> files)
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mBucketFacade->addFilesListByBucketId(mBucketFacade->currentBucket()->id(), files);
    getFilesInfoForNextBucket();
}
