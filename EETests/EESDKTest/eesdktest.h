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
    void testCreateBucket();
    void testDestroyBucket(QString id);
    void testGetBuckets();
    void testUploadFile();

    void showTestStatus(QString method, QString status = "Test succeed: ", QString message = "Ok");

private slots:
    void bucketCreated(EEBucket);
    void bucketDeleted(QString id);
    void bucketsReceived(QList<EEBucket>);

    void requestError(EEError, QString);

private:
    EESDK *mSdk;
    EEFileLoader *mLoader;
    EEDataManager *mDataManager;

    QString mBacketName;

    QList<EEBucket> mBuckets;
    int mDestroyCounter;
};

#endif // EESDKTEST_H
