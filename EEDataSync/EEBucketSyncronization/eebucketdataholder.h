#ifndef EEBUCKETHANDLER_H
#define EEBUCKETHANDLER_H

#include "EEContainers/eebucket.h"
#include "EEContainers/eefile.h"

#include <QObject>
#include <QList>
#include <QMap>

class EEBucket;
/**
 * @brief The EEBucketDataHolder class
 * Class, which holds/manage buckets list and information about it,
 * which are in use right now
 */
class EEBucketDataHolder : public QObject
{
    Q_OBJECT
public:
    explicit EEBucketDataHolder(QObject *parent = nullptr);

    ~EEBucketDataHolder();

    QList<EEBucket*> allBuckets() const;
    void setAllBuckets(QList<EEBucket*> bucketList);
    void addBucketToAllBuckets(EEBucket *newBucket);

    void setCurrentFileIndex(int index);
    int currentFileIndex() const;
    void increaseCurrentIndex();
    void setCurrentBucket(EEBucket *bucket);

    EEBucket *rootBucket() const;
    EEBucket *currentBucket() const;
    EEBucket *bucketById(QString bucketId) const;
    EEBucket *bucketByName(QString bucketName) const;

    bool removeBucketByBucketId(QString bucketId);

    void setWorkingDirectory(QString &directory);
    QString workingDirectory() const;

    void addFilesListByBucketId(QString bucketId, QList<EEFile> files);
    QList<EEFile>filesByBucketId(QString bucketId);
    bool setNextCurrentBucket();
private:
    int mCurrentFileIndex = -1;
    int mCurrentBucketIndex = -1;
    EEBucket *mCurrentBucket;
    QList<EEBucket*> mAllBuckets;
    QMap<QString, QList<EEFile>> mBucketsFiles;
    QString mWorkingDirectory = "";
};

#endif // EEBUCKETHANDLER_H
