#include "eesdktest.h"

#include "EESDK/eefileloader.h"
#include "EEDataSync/eedatamanager.h"

#include <QDebug>
#include <QDateTime>
#include <QDir>

EESDKTest::EESDKTest(QObject *parent) : QObject(parent),
    mSdk{new EESDK()},
    mBacketName{QString().number(QDateTime::currentMSecsSinceEpoch())}
{
    mLoader = new EEFileLoader(mSdk);
    mDataManager = new EEDataManager(mSdk, mLoader);

    mSdk->setCredentials("", "");
//    connect(mSdk, SIGNAL(bucketCreated(EEBucket)), this, SLOT(bucketCreated(EEBucket)));
//    connect(mSdk, SIGNAL(bucketsReceived(QList<EEBucket>)), this, SLOT(bucketsReceived(QList<EEBucket>)));
//    connect(mSdk, SIGNAL(bucketDeleted(QString)), this, SLOT(bucketDeleted(QString)));

//    connect(mSdk, SIGNAL(requestError(EEError, QString)), this, SLOT(requestError(EEError, QString)));

}

EESDKTest::~EESDKTest()
{
    delete mDataManager;
    delete mLoader;
    delete mSdk;
}

void EESDKTest::startTests()
{
    QString lPath = QDir::homePath() + "/Documents/tests";
    mDataManager->initStorage(lPath);

//    QString lPathRestore = QDir::homePath() + "/Documents/restore";
//    mDataManager->initLocalFromCloud(lPathRestore);
//    testCreateBucket();
//    testGetBuckets();
//    testUploadFile();
//    QString lBucketId = "956D8C4E982E32654366DF7F";
//    QString lFileId = "/home/lev-volodymyrrybak/Pictures/corn.jpg";
//    mLoader->uploadData(lBucketId, lFileId);
}

void EESDKTest::testCreateBucket()
{
    qDebug() << "bucket name: " << mBacketName;
    mSdk->createBucket(mBacketName);
}

void EESDKTest::testDestroyBucket(QString id)
{
    mSdk->destroyBucket(id);
}

void EESDKTest::testGetBuckets()
{
    mSdk->getBuckets();
}

void EESDKTest::testUploadFile()
{
    showTestStatus(MTD_UPLOAD_SHARD);
}

void EESDKTest::showTestStatus(QString method, QString status, QString message)
{
    qDebug() << status << method << " : " << message;

    if (method == MTD_CREATE_BUCKET) {
        qDebug() << "Buckets created!";
        foreach (EEBucket bucket, mBuckets) {
            qDebug() << bucket.id() << " " << bucket.name();
        }
    } else if (method == MTD_DESTROY_BUCKET) {
        if (mDestroyCounter == 0) {
            ++mDestroyCounter;
        }
    } else if (method == MTD_GET_BUCKETS) {
        qDebug() << "Buckets received" << mBuckets.count();
        foreach (EEBucket bucket, mBuckets) {
            qDebug() << bucket.id() << " " << "Starting donwloading";
            //mLoader->downloadFile(bucket.id(), "6a285fbdaac9c30774714891");
            //6a285fbdaac9c30774714891
            //2f01c8355f7e20ccfa0c9c68
        }
    } else if (method == MTD_UPLOAD_SHARD) {
        QString bucketId = "a1843c155ebf939aa5996c7a";
        QString filePath = "/home/lev-volodymyrrybak/Pictures/corn.jpg";

        qDebug() << bucketId << " " << "Starting uploading";
        mLoader->uploadData(bucketId, filePath);

//        foreach (EEBucket bucket, mBuckets) {
//            qDebug() << bucket.id() << " " << "Starting donwloading";
//            mLoader->downloadFile(bucket.id(), filePath);
//        }

    }
}

//handle response
void EESDKTest::bucketCreated(EEBucket newBucket)
{
    bool lIsAlreadyExists = false;
    qDebug() << "Bucket has been created!";

    foreach (EEBucket bucket, mBuckets) {
        if (bucket == newBucket) {
            qDebug() << "Bucket is already on the list";
            lIsAlreadyExists = true;
            break;
        }
    }

    if (!lIsAlreadyExists) {
        mBuckets.append(newBucket);
    }
}

void EESDKTest::bucketDeleted(QString)
{
    showTestStatus(MTD_DESTROY_BUCKET);
}

void EESDKTest::bucketsReceived(QList<EEBucket> b)
{
    mBuckets = b;

    showTestStatus(MTD_GET_BUCKETS);
}

void EESDKTest::requestError(EEError err, QString method)
{
    showTestStatus(method, "Request error: ", err.message());
}
