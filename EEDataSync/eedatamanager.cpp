#include "eedatamanager.h"
#include "eefolderparsecontroller.h"
#include "eefoldermodel.h"
#include "eebucketwrapper.h"
#include "crypto.h"
#include "controller.h"

#include "eesdk.h"
#include "EESDK/eefileloader.h"

#include <QFile>
#include <QSettings>
#include <QApplication>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>

#define CLOUD_CONFIG "cloud.json"

#define CONFIG_DIR(x)      QDir::homePath() + QString(".config/xcloud/") + x

EEDataManager::EEDataManager(EESDK *sdk, EEFileLoader *loader,QObject *parent) :
    QObject(parent),
    mSdk{sdk},
    mLoader{loader},
    mFolderParseController{nullptr},
    mBucketWrapper{new EEBucketWrapper}

{
    connect(mSdk, SIGNAL(bucketCreated(EEBucket)), this, SLOT(bucketCreated(EEBucket)));

    connect(mSdk, SIGNAL(bucketsReceived(QList<EEBucket>)), this, SLOT(bucketsListReceived(QList<EEBucket>)));
    connect(mSdk, SIGNAL(filesForBucketReceived(QList<EEFile>)), this, SLOT(startFilesNamesDecryption(QList<EEFile>)));
    connect(mLoader, SIGNAL(fileUploaded()), this, SLOT(startDataUploading()));
    connect(mLoader, SIGNAL(moveToTheNextFile()), this, SLOT(startDataUploading()));
    connect(mLoader, SIGNAL(fileDownloaded()), this, SLOT(startDataDownloading()));


    connect(this, &EEDataManager::startToDownload, this, [this] () {
        qDebug() << "Start to load data...";
        QString lPathRestore = QDir::homePath() + "/Documents/restore";
        initLocalFromCloud(lPathRestore);
    }, Qt::QueuedConnection);

}

EEDataManager::~EEDataManager()
{
    if (mFolderParseController != nullptr) {
        delete mFolderParseController;
    }
    delete mBucketWrapper;
}

/**
 * @brief EEDataManager::initStorage
 * Build bucket queue from root folder
 * then start to upload them to the cloud
 * @param folder
 */
void EEDataManager::initStorage(QString folder)
{
    mFolderParseController = new EEFolderParseController(folder);
    EEFolderModel *mRoot = mFolderParseController->rootModel();

    mBucketWrapper->addElementToUploadBucketQueue(mRoot);
    buildUploadingDataQueue(mRoot);

    startUploadNextBucket();
}

void EEDataManager::initLocalFromCloud(QString folder)
{
    qDebug() << "- EEDataManager::initLocalFromCloud";
    mBucketWrapper->setRestoreDirectory(folder);
    mSdk->getBuckets();
}
/**
 * @brief EEDataManager::buildDataQueue
 * Build queue of buckets. Using in order for folder tree
 * Folder equal to  bucket
 * @param folder
 */
void EEDataManager::buildUploadingDataQueue(EEFolderModel * const folder)
{
    foreach (EEFolderModel *folder, folder->getFolderList()) {
        mBucketWrapper->addElementToUploadBucketQueue(folder);
        buildUploadingDataQueue(folder);
    }
}
/**
 * @brief EEDataManager::createBucket
 * Create bucket according to current bucket in usage
 */
void EEDataManager::createBucket()
{
    qDebug() << "- EEDataManager::createBucket";
    qDebug() << "Folder name: " << mBucketWrapper->currentFolderName();
    //create bucket for folder
    QString lBucketName = mBucketWrapper->currentFolderName();

    mSdk->createBucket(lBucketName);
}
/**
 * @brief EEDataManager::uploadFile
 * Upload file to currently using bucket
 */
void EEDataManager::uploadFile()
{
    qDebug() << "- EEDataManager::uploadFile";
    QString lFileName = mFolderParseController->filePath(mBucketWrapper->currentFileNameFromFolderModel());
    QString lBucketId = mBucketWrapper->currentBucket().id();
    qDebug() << "File name to upload : " << lFileName;
    qDebug() << "Bucket id: " << lBucketId;
    mLoader->uploadData(lBucketId, lFileName);
}
/**
 * @brief EEDataManager::startUseNextBucket
 * Move to the next bucket. Reset file index and send request for bucket creation
 */
#include <QThread>
void EEDataManager::startUploadNextBucket()
{
    if (mBucketWrapper->setCurrentFolderModelByBucket()) {
        mBucketWrapper->setCurrentFileIndex(-1);
        //clean_values();
        createBucket();
    } else {
        qDebug() << "All buckets for current folder have been built!";
#warning temporary
        emit startToDownload();
        //clean_values();
//        QThread thread;
//        thread.c
        //clean_uploading_data();
//        qDebug() << "Start to load data...";
//        QString lPathRestore = QDir::homePath() + "/Documents/restore";
//        initLocalFromCloud(lPathRestore);
    }
}

/**
 * @brief EEDataManager::startDataUploading
 * Control uploading of the files into bucket
 * and moving to the next one, of needed
 */
void EEDataManager::startDataUploading()
{
    qDebug() << "- EEDataManager::startDataUploading";
    int lNumberOfFiles = mBucketWrapper->currentFolderSize();
    if (mBucketWrapper->currentFileIndex() + 1 < lNumberOfFiles) {
        qDebug() << "Move to the next file";
        //move to the next file
        mBucketWrapper->increaseCurrentIndex();
        uploadFile();
    } else {
        qDebug() << "Move to the next bucket";
        startUploadNextBucket();
    }
}

/**
 * @brief EEDataManager::bucketCreated
 * Receive signal about bucket creation
 * and start to upload its data(files)
 * @param newbucket
 */
void EEDataManager::bucketCreated(EEBucket newbucket)
{
    qDebug() << "- EEDataManager::bucketCreated";

    mBucketWrapper->addBucketToAllBuckets(newbucket);
    mBucketWrapper->setCurrentBucket(newbucket);

    startDataUploading();
    qDebug() << "Uploaded";
}
/**
 * @brief EEDataManager::bucketsListReceived
 * Slot, which activate after receiving list of buckets.
 * Initialize list of buckets and queue of buckets to download
 * Start process of downloading
 * @param buckets - list of buckets
 */
void EEDataManager::bucketsListReceived(QList<EEBucket> buckets)
{
    qDebug() << "- EEDataManager::bucketsListReceived(QList<EEBucket> buckets)";
    mBucketWrapper->setAllBuckets(buckets);
    mBucketWrapper->initializeDownloadingQueue();
    startUseNextBucketForDownloading();
}
/**
 * @brief EEDataManager::getFilesListForCurrentBucket
 * Create directory for current bucket, if needed
 * Send get request for this bucket files
 */
void EEDataManager::getFilesListForCurrentBucket() {
    qDebug() << "- EEDataManager::getFilesListForCurrentBucket()";
    //create folder, if needed
    QString lPath = mBucketWrapper->restoreDirectory() + mBucketWrapper->currentBucket().name();
    QDir lDir(lPath);
    if (!lDir.exists()) {
        lDir.mkpath(".");
    }

    mSdk->getFilesFromBucket(mBucketWrapper->currentBucket().id());
}
/**
 * @brief EEDataManager::filesListReceived
 * This method starts to using after receiving list of file
 * for current bucket and decryption of their names.
 * Call another method to start process of files downloading
 * @param files - list of files of backet
 */
void EEDataManager::filesListReceived(QList<EEFile> files)
{
    qDebug() << "- EEDataManager::filesListReceived(QList<EEFile> files)";
    mBucketWrapper->setCurrentFileList(files);
    mBucketWrapper->setCurrentFileIndex(-1);

    startDataDownloading();
}
/**
 * @brief EEDataManager::startFilesNamesDecryption
 * Slot, which activates after receiving list of file for current bucket
 * then decrypt list of files names
 * using .c decryption
 * @param files - list of files
 */
void EEDataManager::startFilesNamesDecryption(QList<EEFile> files)
{
    qDebug() << "- EEDataManager::startFilesNamesDecryption(QList<EEFile> files)";
    for (auto &file : files) {
        QString lFileName = file.filename();
        EEBucket lFileBucket = mBucketWrapper->bucketById(file.bucketId());
        QByteArray lMnenmonicBa = lFileBucket.encryptionKey().toLatin1();
        QByteArray lBucketIdBa = lFileBucket.id().toUpper().toLatin1();
        QByteArray lFileNameBa = lFileName.toLatin1();

        const char *lMnenmonic = lMnenmonicBa.constData();
        const char *lBucketId = lBucketIdBa.constData();
        const char *lFileNameChar = lFileNameBa.constData();
        char *result;

        if (!decrypt_file_name(lMnenmonic, lBucketId, lFileNameChar, &result)) {
            qDebug() << "File name has been succesfully decrypted!";
            lFileName = QString::fromUtf8(result);
        } else {
            qDebug() << "File name couldn't be decrypted!";
            lFileName = "Unknown_Name";
        }
        file.setFilename(lFileName);
    }

    filesListReceived(files);
}
/**
 * @brief EEDataManager::startUseNextBucketForDownloading
 * Take first element of queue and set it as currently on usage
 */
void EEDataManager::startUseNextBucketForDownloading()
{
    qDebug() << "- EEDataManager::startUseNextBucketForDownloading()";
    if (mBucketWrapper->setCurrentBucketByDownloadingQueue()) {
        getFilesListForCurrentBucket();
    } else {
        qDebug() << "All files for current bucket have already been downloaded!";
    }
}
/**
 * @brief EEDataManager::startDataDownloading
 * Download files for current bucket
 * or if all files have already been downloaded
 * move to the next bucket
 */
void EEDataManager::startDataDownloading()
{
    int lNumberOfFiles = mBucketWrapper->currentFilesListSize();
    if (mBucketWrapper->currentFileIndex() + 1 < lNumberOfFiles) {
        qDebug() << "Move to the next file";
        //move to the next file
        mBucketWrapper->increaseCurrentIndex();
        downloadFile();
    } else {
        qDebug() << "Move to the next bucket";
        startUseNextBucketForDownloading();
    }
}

/**
 * @brief EEDataManager::downloadFile
 * Download file into restore library
 */
void EEDataManager::downloadFile()
{
    qDebug() << "- EEDataManager::downloadFile";
    EEBucket lCurrentBucket = mBucketWrapper->currentBucket();
    QString lFileId = mBucketWrapper->currentFile().id();
    QString lFileName = mBucketWrapper->currentFile().filename();
    QString lPath = mBucketWrapper->restoreDirectory() + lCurrentBucket.name() + '/' + lFileName;

    qDebug() << "Path to save : " << lPath;
    mLoader->downloadFile(lCurrentBucket.id(), lFileId, lPath);
}
