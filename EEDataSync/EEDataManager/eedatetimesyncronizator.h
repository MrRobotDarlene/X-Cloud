#ifndef EEDATESYNCRONIZATOR_H
#define EEDATESYNCRONIZATOR_H

#include <QObject>
class EEFolderModel;
class EEFolderParseController;
class EESDK;
class EEBucketFacade;

/**
 * @brief The EEDateTimeSyncronizator class
 * Class, used to syncronizate timestamps on local data
 * with cloud creation date
 */
class EEDateTimeSyncronizator : public QObject
{
    Q_OBJECT
public:
    explicit EEDateTimeSyncronizator(EESDK *sdk, EEBucketFacade *facade, EEFolderParseController *folderParseController, QObject *parent = nullptr);

    /**
     * @brief EEDateTimeSyncronizator::initializeDateSyncronization
     * Start process of file's/folder's date syncronization.
     * This method calls after all required data were donwloaded/uploaded
     */
    void initializeDateSyncronization();
    /**
     * @brief EEDateTimeSyncronizator::syncronizeLocalDataWithCloud
     * Reinitialized data about local folders/files, start to compare
     * with cloud data
     */
    void syncronizeLocalDataWithCloud();

private:

    /**
    * @brief EEDateTimeSyncronizator::moveThrowFolder
    * Moves recursively throw folder, syncronizate local files last modification date with cloud date
    * @param model
    */
    void moveThrowFolder(EEFolderModel *model);

    /**
     * @brief EEDateTimeSyncronizator::setFileModificationTime
     * Manually set file modification time
     * @param fileName
     * @param updateTime
     */
    bool setFileModificationTime(QString fileName, QDateTime updateTime);
signals:

public slots:
private:
    EESDK *mSdk;
    EEFolderParseController *mFolderParseController;
    EEBucketFacade *mBucketFacade;
};

#endif // EEDATESYNCRONIZATOR_H
