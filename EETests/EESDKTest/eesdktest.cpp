#include "eesdktest.h"

#include "EESDK/eefileloader.h"
#include "EEDataSync/EEDataManager/eedatamanager.h"

#include <QDebug>
#include <QDateTime>
#include <QDir>

EESDKTest::EESDKTest(QObject *parent) : QObject(parent),
    mSdk{new EESDK()},
    mBacketName{QString().number(QDateTime::currentMSecsSinceEpoch())}
{
    //for registration
//    QString lPass = "Qwerty123";
//    QString lPassword = QString(QCryptographicHash::hash((lPass.toUtf8()),QCryptographicHash::Sha256).toHex());
//    mSdk->registerUser("indeema.qateam@gmail.com",  "Qwerty123");

    mSdk->setCredentials("indeema.qateam@gmail.com", "Qwerty123");//("user1@test.com", "asdf1234");

    mLoader = new EEFileLoader(mSdk);
//    mDataManager = new EEDataManager(mSdk, mLoader);
//    mDataManager->initializeDateSyncronization();

//    connect(mSdk, SIGNAL(bucketCreated(EEBucket*)), this, SLOT(bucketCreated(EEBucket*)));
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
//    testDataDownloading();
//    testDataUploading();
//    testCreateBucket();
    testGetBuckets();
//    testDestroyBuckets();
//    testDeleteFile();
//    testGetListOfFilesForBucket();
//    testUploadFile();
//    testDownloadFile();
}

void EESDKTest::testDataDownloading()
{
    QString lPathRestore = QDir::homePath() + "/Documents/restore";
    mDataManager->initLocalFromCloud();

}

void EESDKTest::testDataUploading()
{
    QString lPath = QDir::homePath() + "/Documents/tests";
    mDataManager->initStorage();

}

void EESDKTest::testCreateBucket()
{
    qDebug() << "bucket name: " << mBacketName;
    mSdk->createBucket(mBacketName);
}

void EESDKTest::testDestroyBuckets()
{
    connect(mSdk, SIGNAL(bucketsReceived(QList<EEBucket*>)), this, SLOT(bucketsForDeletionReceived(QList<EEBucket*>)));
    testGetBuckets();
}

void EESDKTest::testGetBuckets()
{
    mSdk->getBuckets();
}

void EESDKTest::testUploadFile()
{
    showTestStatus(MTD_UPLOAD_SHARD);
}

void EESDKTest::testDownloadFile()
{
    showTestStatus(MTD_DOWNLOD_SHARD);
}

#include <QCryptographicHash>
void EESDKTest::testUserRegistration()
{
    //for registration
    QString lPass = "Qwerty123";
    QString lPassword = QString(QCryptographicHash::hash((lPass.toUtf8()),QCryptographicHash::Sha256).toHex());
    mSdk->registerUser("indeema.qateam@gmail.com",  "Qwerty123");
}

void EESDKTest::testGetListOfFilesForBucket()
{
    showTestStatus(MTD_GET_FILES_FOR_BUCKETS);
}

void EESDKTest::testDeleteFile()
{
    showTestStatus(MTD_DELETE_FILE_FROM_BUCKET);
}


void EESDKTest::showTestStatus(QString method, QString status, QString message)
{
    qDebug() << status << method << " : " << message;

    if (method == MTD_CREATE_BUCKET) {
        qDebug() << "Buckets created!";
        foreach (EEBucket bucket, mBuckets) {
            qDebug() << bucket.id() << " " << bucket.name();
        }
    } else if (method == MTD_GET_BUCKETS) {
        qDebug() << "Buckets received" << mBuckets.count();
        foreach (EEBucket bucket, mBuckets) {
            qDebug() << bucket.id() << " ";

            //testUploadFile();
            //testDownloadFile();
        }
    } else if (method == MTD_UPLOAD_SHARD) {
        qDebug() << "Uploading file..";
        QString lBucketId = "cf45664ec92607548dea94af";
        QString lFilePath = "/Users/vshevchyk/Documents/test";
        mLoader->uploadData(lBucketId, lFilePath);

//        foreach (EEBucket bucket, mBuckets) {
//            qDebug() << bucket.id() << " " << "Starting uploading";
//            mLoader->uploadData(bucket.id(), lFilePath);
//        }

    } else if (method == MTD_DOWNLOD_SHARD) {
        qDebug() << "Starting downloading...";
        QString lBucketId = "cf45664ec92607548dea94af";
        QString lFileId = "6a285fbdaac9c30774714891";
        //6a285fbdaac9c30774714891
        //2f01c8355f7e20ccfa0c9c68
        QString filePath = "/Users/vshevchyk/Documents/test";

        mLoader->downloadFile(lBucketId, lFileId, filePath);
//        foreach (EEBucket bucket, mBuckets) {
//            qDebug() << bucket.id() << " " << "Starting downloading";
//            mLoader->downloadFile(bucket.id(), lFileId, filePath);
//        }
    } else if (method == MTD_DESTROY_BUCKET) {
        qDebug() << "Destroying buckets...";
        foreach (EEBucket *bucket, mBuckets) {
            qDebug() << bucket->id() << " " << "Destroying buckets";
            mSdk->destroyBucket(bucket->id());
        }
    } else if (method == MTD_GET_FILES_FOR_BUCKETS ) {
        QString lBucketId = "38cc0a7946e5d777e0cf7a14";
        mSdk->getFilesFromBucket(lBucketId);
    } else if (method == MTD_DELETE_FILE_FROM_BUCKET) {
        QString lBucketId = "EDD84AC3ED8CDB500F736F5F";
        QString lFileId = "49B2C4340ABAEC0A79FDE7F7";
        mLoader->deleteFile(lBucketId, lFileId);
    }
}

//handle response
void EESDKTest::bucketCreated(EEBucket *newBucket)
{
    bool lIsAlreadyExists = false;
    qDebug() << "Bucket has been created!";

    foreach (EEBucket *bucket, mBuckets) {
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

void EESDKTest::bucketsReceived(QList<EEBucket*> b)
{
    mBuckets = b;
}

void EESDKTest::bucketsForDeletionReceived(QList<EEBucket *>b)
{
    mBuckets = b;
    qDebug() << "Buckets list for deletion...";
    showTestStatus(MTD_GET_BUCKETS);
    showTestStatus(MTD_DESTROY_BUCKET);
}

void EESDKTest::requestError(EEError err, QString method)
{
    showTestStatus(method, "Request error: ", err.message());
}
