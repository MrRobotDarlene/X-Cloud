#include "eedatetimesyncronizator.h"
#include "EEDataSync/EEParser/eefoldermodel.h"
#include "EEContainers/eebucket.h"
#include "EEDataSync/EEParser/eefolderparsecontroller.h"
#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "eesdk.h"
#include "eestatustimer.h"


//for file time modification
#include <sys/types.h>
#include <sys/time.h>
#include <utime.h>
#include <time.h>

EEDateTimeSyncronizator::EEDateTimeSyncronizator(EESDK *sdk, EEBucketFacade *facade, EEFolderParseController *folderParseController, QObject *parent)
    : QObject(parent),
      mSdk{sdk},
      mFolderParseController{folderParseController},
      mBucketFacade{facade}
{

}


/**
 * @brief EEDateTimeSyncronizator::initializeDateSyncronization
 * Start process of file's/folder's date syncronization.
 * This method calls after all required data were donwloaded/uploaded
 */
void EEDateTimeSyncronizator::initializeDateSyncronization()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
    mSdk->getBuckets();
}
/**
 * @brief EEDateTimeSyncronizator::syncronizeLocalDataWithCloud
 * Reinitialized data about local folders/files, start to compare
 * with cloud data
 */
void EEDateTimeSyncronizator::syncronizeLocalDataWithCloud()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    EEFolderModel *lLocalFolderRoot = nullptr;


    qDebug() << "Scanning local folders...";
    mFolderParseController->startSubfoldersInitialization(mBucketFacade->workingDirectory());
    lLocalFolderRoot = mFolderParseController->rootModel();

    moveThrowFolder(lLocalFolderRoot);

    //restart timer again
    EEStatusTimer::getInstance().startTimer();
}

/**
* @brief EEDateTimeSyncronizator::moveThrowFolder
* Moves recursively throw folder, syncronizate local files last modification date with cloud date
* @param model
*/
void EEDateTimeSyncronizator::moveThrowFolder(EEFolderModel *model)
{
   qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

   EEBucket* lBucket = mBucketFacade->bucketByName(model->name());

   if (lBucket != nullptr) {
       QList<EEFile>lCloudFiles =  mBucketFacade->filesByBucketId(lBucket->id());
       QString lBucketName = lBucket->name();
       QString lSeparator = "/";
#ifdef WIN32
       lSeparator = "\\";
#endif
       if (lBucketName.at(0) != lSeparator) {
           lBucketName = lSeparator + lBucketName;
       }

       QString lFilePath;
       //special case for root
       if (lBucketName == "/") {
           lFilePath = mBucketFacade->workingDirectory() + lBucketName;
       } else {
           lFilePath = mBucketFacade->workingDirectory() + lBucketName + lSeparator;
       }

       for (auto localFile : model->filesList()) {
           // if time was succesfully modified, set in the program time
           for (auto cloudFile : lCloudFiles) {
               QString lCloudPath = lFilePath + cloudFile.filename();
               QString lLocalPath = mBucketFacade->workingDirectory() + localFile->name();
               if (lCloudPath == lLocalPath) {
                   if (setFileModificationTime( lLocalPath, cloudFile.created())) {
                       localFile->setUpdated(cloudFile.created());
                   }
               }
           }
       }

       for (auto folder : model->folderList()) {
           moveThrowFolder(folder);
       }
   } else {
       qDebug() << "Error! Bucket wasn't initialized!!!" << model->name();
   }
}

/**
 * @brief EEDateTimeSyncronizator::setFileModificationTime
 * Manually set file modification time
 * @param fileName
 * @param updateTime
 */
bool EEDateTimeSyncronizator::setFileModificationTime(QString fileName, QDateTime updateTime)
{
    bool lIsModified = false;
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    tzset();

    int lHour =  updateTime.toLocalTime().time().hour();
    int lMin = updateTime.toLocalTime().time().minute();
    int lSec = updateTime.toLocalTime().time().second();

    updateTime.setTime(QTime(lHour, lMin, lSec));
    updateTime.setTimeSpec(Qt::LocalTime);


    constexpr int lYearDifference = 1900;
    tm lTimeInformation = {0};
    utimbuf lTimerModifier;

    lTimeInformation.tm_year = updateTime.date().year() - lYearDifference;
    lTimeInformation.tm_mon = updateTime.date().month() - 1;
    lTimeInformation.tm_mday = updateTime.date().day();
    lTimeInformation.tm_hour = updateTime.time().hour();
    lTimeInformation.tm_min = updateTime.time().minute();
    lTimeInformation.tm_sec = updateTime.time().second();
    lTimeInformation.tm_isdst = -1;

    lTimerModifier.modtime = mktime(&lTimeInformation);

    QByteArray lFileNameBa = fileName.toLatin1();
    const char *lFileName = lFileNameBa.constData();

    // Show file time before and after
    if (utime( lFileName, &lTimerModifier ) == -1) {
        qDebug() << "File time cannot be modified!";
    } else {
        qDebug() << "File time succesfully modified! File name : " << lFileName;
        lIsModified = true;
    }
    return lIsModified;
}
