#ifndef EEDATATREEBUILDER_H
#define EEDATATREEBUILDER_H

#include <QObject>

class EEBucketFacade;
class EESDK;

/**
 * @brief The EEDataTreeBuilder class
 * Class, used to build tree with folders with their files
 * based on cloud buckets with files
 */
class EEDataTreeBuilder : public QObject
{
    Q_OBJECT
public:
    explicit EEDataTreeBuilder(EESDK *sdk, EEBucketFacade *facade, QObject *parent = nullptr);

    /**
     * @brief EEDataManager::getFilesInfoForNextBucket
     * download files by id for bucket!
     * After download - start to check data statuses
     */
    void getFilesInfoForNextBucket();

signals:
    void cloudTreeReceived();
public slots:
    /**
     * @brief EEDataManager::startBucketsStatusCheck
     * Start to check folder status.
     * Activates after timer timeout.
     */
    void startBucketsStatusCheck();
private:
    EESDK *mSdk;
    EEBucketFacade *mBucketFacade;
};

#endif // EEDATATREEBUILDER_H
