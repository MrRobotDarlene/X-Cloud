#ifndef EEBUCKETHANDLER_H
#define EEBUCKETHANDLER_H


#include <QObject>
#include <QList>
#include <QMap>

class EEBucket;
class EEFile;
class EEFolderModel;

/**
 * @brief The EEBucketDataHolder class
 * Class, which holds/manage buckets and files information
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
    void setCurrentFileName(QString file);


    EEBucket *rootBucket() const;
    EEBucket *currentBucket() const;
    EEBucket *bucketById(QString bucketId) const;
    EEBucket *bucketByName(QString bucketName) const;

    QString currentFileName() const;

    bool removeBucketByBucketId(QString bucketId);

    void setWorkingDirectory(QString &directory);
    QString workingDirectory() const;

    void addFilesListByBucketId(QString bucketId, QList<EEFile *> files);
    QList<EEFile*>filesByBucketId(QString bucketId);
    EEFile *bucketFileByName(QString fileName, QString bucketId);
    bool removeFileFromBucket(QString fileName, QString bucketId);
    bool removeBucketFromTreeById(QString bucketId);
    bool setNextCurrentBucket();


    void buildLocalFormFromBucket();

    EEFolderModel *bucketsFolderModelForm() const;
private:
    int mCurrentFileIndex;
    int mCurrentBucketIndex;
    QString mCurrentFileName;
    EEBucket *mCurrentBucket;
    QList<EEBucket*> mAllBuckets;
    //bucket id as key, and list of files for that bucket as value
    QMap<QString, QList<EEFile*>> mBucketsFiles;
    QString mWorkingDirectory;


    EEFolderModel *mBucketsFolderModelForm;
};

#endif // EEBUCKETHANDLER_H
