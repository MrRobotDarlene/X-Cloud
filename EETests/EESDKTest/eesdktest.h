#ifndef EESDKTEST_H
#define EESDKTEST_H

#include "EESDK/eesdk.h"

class EEFileLoader;
class EEDataManager;


class EESDKTest : public QObject
{
    Q_OBJECT
public:
    EESDKTest(QObject *parent = nullptr);
    ~EESDKTest();
    void startTests();

private:
    void testDataDownloading();
    void testDataUploading();
    void testCreateBucket();
    void testDestroyBuckets();
    void testGetBuckets();
    void testUploadFile();
    void testDownloadFile();
    void testUserRegistration();
    void testGetListOfFilesForBucket();
    void testDeleteFile();

    void showTestStatus(QString method, QString status = "Test succeed: ", QString message = "Ok");

private slots:
    void bucketCreated(EEBucket*);
    void bucketsReceived(QList<EEBucket*>);
    void bucketsForDeletionReceived(QList<EEBucket*>);


    void requestError(EEError, QString);

private:
    EESDK *mSdk;
    EEFileLoader *mLoader;
    EEDataManager *mDataManager;

    QString mBacketName;

    QList<EEBucket*> mBuckets;
    int mDestroyCounter;
};

#endif // EESDKTEST_H
