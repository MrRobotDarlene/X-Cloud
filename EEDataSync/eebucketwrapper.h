#ifndef EEREMOTEBUCKETCONTROLLER_H
#define EEREMOTEBUCKETCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QString>

#include "EESDK/EEContainers/eebucket.h"
#include "EESDK/EEContainers/eefile.h"

class EEFolderModel;

/**
 * @brief The EEBucketWrapper class
 * Class-wrapper for buckets(folders) and files.
 * Used by EEDataManager to control data for
 * uploading and downloading.
 * Created for convinience.
 */
class EEBucketWrapper : public QObject
{
    Q_OBJECT
public:
    explicit EEBucketWrapper(QObject *parent = nullptr);

    //for uploading
    void addElementToUploadBucketQueue(EEFolderModel *folder);
    QQueue<EEFolderModel *> uploadingBucketsQueue() const;
    bool setCurrentFolderModelByBucket();
    EEFolderModel *currentFolderModel() const;
    QString currentFolderName() const;
    int currentFolderSize() const;
    QString currentFileNameFromFolderModel() const;

    //for both
    QList<EEBucket> allBuckets() const;
    void setAllBuckets(QList<EEBucket> bucketList);
    void addBucketToAllBuckets(EEBucket newBucket);
    void setCurrentFileIndex(int index);
    int currentFileIndex() const;
    void increaseCurrentIndex();
    void setCurrentBucket(EEBucket bucket);
    EEBucket currentBucket() const;
    EEBucket bucketById(QString bucketId) const;

    //for downloading
    void initializeDownloadingQueue();
    QQueue<EEBucket> downloadingBucketsQueue() const;
    bool setCurrentBucketByDownloadingQueue();
    void setCurrentFileList(QList<EEFile> files);
    int currentFilesListSize();
    EEFile currentFile() const;
    void setRestoreDirectory(QString directory);
    QString restoreDirectory() const;

private:
    //for both
    int mCurrentFileIndex;
    EEBucket mCurrentBucket;
    QList<EEBucket> mAllBuckets;

    //for downloading
    QQueue<EEBucket>mDownloadingBucketQueue;
    QList<EEFile>mCurrentFileList;
    QString mRestoreDirectory;

    //for uploading
    QQueue<EEFolderModel*>mUploadingBucketQueue;
    EEFolderModel *mCurrentFolderModel;

};

#endif // EEREMOTEBUCKETCONTROLLER_H
