#include "eedatetimesyncronizator.h"
#include "EEDataSync/EELocalDataParser/eefoldermodel.h"
#include "EESDK/EEContainers/eebucket.h"
#include "eefolderparsecontroller.h"
#include "eejsonbuilder.h"

#include "EEDataSync/EEBucketSyncronization/eebucketfacade.h"
#include "EESDK/eesdk.h"
#include "eestatustimer.h"
#include "EESDK/EEContainers/eefile.h"

#include <QDateTime>
#include <QDebug>


#include <typeinfo>

//for file time modification
#include <sys/types.h>
#include <sys/time.h>
#include <utime.h>
#include <time.h>

EEDateTimeSyncronizator::EEDateTimeSyncronizator(EESDK *sdk,
                                                 EEJsonBuilder *builder,
                                                 EEBucketFacade *facade,
                                                 EEFolderParseController *folderParseController,
                                                 QObject *parent)
    : QObject(parent),
      mSdk{sdk},
      mJsonBuilder{builder},
      mFolderParseController{folderParseController},
      mBucketFacade{facade}
{

}
/**
 * @brief EEDateTimeSyncronizator::initializeFileDateSyncronization
 * This process starts after files uploading
 */
void EEDateTimeSyncronizator::initializeFileDateSyncronization()
{
    qDebug() << "- EEDateTimeSyncronizator::initializeFileDateSyncronization()";

    mSdk->getFilesFromBucket(mBucketFacade->currentBucket()->id());
}
/**
 * @brief EEDateTimeSyncronizator::synchronizateFileDateFile
 * Syncronizate datetime for current bucket and file.
 * Activates after downloading and uploading
 */
void EEDateTimeSyncronizator::synchronizateFileDateTime()
{
    qDebug() << "- EEDateTimeSyncronizator::syncFileDateFile()";
    EEFile *lFile = mBucketFacade->bucketFileByName(mBucketFacade->currentFileName(),
                                                    mBucketFacade->currentBucket()->id());
    if (lFile != nullptr) {
        qDebug() << "Syncronization of files' date for folder";
        QString lSeparator = "/";
        QString lBucketName = mBucketFacade->currentBucket()->name();
        if (lBucketName.at(0) != lSeparator ) {
            lBucketName = "/" + lBucketName;
        }
        QString lFilePath =  mBucketFacade->workingDirectory() + lBucketName;
        if (lBucketName != lSeparator) {
            lFilePath += "/";
        }

        //if file time was succesfully modified - update its status in .json file
        if (setFileModificationTime(lFilePath + lFile->filename(), lFile->created())) {
            qDebug() << "File time was succesfully modified! Update status...";
            mJsonBuilder->updateCurrentElementStatus(lFile->created());
        }
    } else {
        qDebug() << "No such file in bucket's files list!!!" << mBucketFacade->currentFileName();
    }
    emit fileDateTimeSyncronizated();
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
    tm lTimeInformation;
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
        qDebug() << "File time cannot be modified!" << lFileName;
    } else {
        qDebug() << "File time succesfully modified! File name : " << lFileName;
        lIsModified = true;
    }
    return lIsModified;
}


