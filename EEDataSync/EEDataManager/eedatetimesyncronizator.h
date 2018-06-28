#ifndef EEDATESYNCRONIZATOR_H
#define EEDATESYNCRONIZATOR_H

#include <QObject>
#include <QList>


class EEFolderModel;
class EEFolderParseController;
class EESDK;
class EEBucketFacade;
class EEJsonBuilder;

/**
 * @brief The EEDateTimeSyncronizator class
 * Class, used to syncronizate timestamps on local data
 * with cloud creation date
 */
class EEDateTimeSyncronizator : public QObject
{
    Q_OBJECT
public:
    explicit EEDateTimeSyncronizator(EESDK *sdk, EEJsonBuilder *builder, EEBucketFacade *facade, EEFolderParseController *folderParseController, QObject *parent = nullptr);
private:
    /**
     * @brief EEDateTimeSyncronizator::setFileModificationTime
     * Manually set file modification time
     * @param fileName
     * @param updateTime
     */
    bool setFileModificationTime(QString fileName, QDateTime updateTime);

signals:
    void fileDateTimeSyncronizated();

public slots:
    /**
     * @brief EEDateTimeSyncronizator::initializeFileDateSyncronization
     * This process starts after files uploading
     */
    void initializeFileDateSyncronization();

    /**
     * @brief EEDateTimeSyncronizator::synchronizateFileDateFile
     * Syncronizate datetime for current bucket and file.
     * Activates after downloading and uploading
     */
    void synchronizateFileDateTime();
private:
    EESDK *mSdk;
    EEJsonBuilder *mJsonBuilder;
    EEFolderParseController *mFolderParseController;
    EEBucketFacade *mBucketFacade;
};

#endif // EEDATESYNCRONIZATOR_H
