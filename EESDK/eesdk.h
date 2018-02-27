#ifndef EESDK_H
#define EESDK_H

#include "EEContainers/eecontainers.h"

#include <QObject>
#include <QNetworkRequest>
#include <QList>

#define MTD_KEY             "Called method"
#define MTD_GET_BUCKETS     "Get list of buckets"
#define MTD_GET_SHARDS      "Get list of shards"
#define MTD_ADD_SHARDS_TO_FRAME     "Add shard to frame"
#define MTD_GET_FILES_FOR_BUCKETS   "Get files list of buckets"
#define MTD_CREATE_BUCKET   "Create new bucket"
#define MTD_DESTROY_BUCKET  "Destroy bucket for given id"
#define MTD_CREATE_TOKEN    "Create token for bucket"
#define MTD_UPLOAD_SHARD    "Upload shard to node"
#define MTD_DOWNLOD_SHARD   "Download shard from node"

#define MTD_REGISTER_USER   "Register new user"

#define MTD_CREATE_FRAME    "Create frame"


class QNetworkAccessManager;
class QNetworkReply;
class EESDKTest;

class EESDK : public QObject
{
    friend class EESDKTest;
    Q_OBJECT
public:
    explicit EESDK(QObject *parent = nullptr);

    virtual ~EESDK();

    void setCredentials(QString email, QString password);

    QString getEmail() const;

    QString getPassword() const;

    //managing users
    void registerUser(QString email, QString password, QString pubkey = "", QString referalKey = "");

    //working with buckets
    void getBuckets();
    void getFilesFromBucket(QString bucket, QString startDate ="");
    void getFileInfo(QString bucket, QString fileId, QString skip, QString limit, QString exclude);
    void createBucket(QString name, QStringList pubKeys = QStringList(""));
    void destroyBucket(QString id);
    //this is used for pull and push token request
    void createToken(QString id, QString file, QString operation = "");

    void getListOfShards(QString bucketId, QString fileId);
    void addShardToFrame(EEShardRequest, QString);

    //working with frames
    void createFrame();

    void uploadShardToNode(EEShard shard, QByteArray data);
    void downloadShardFromNode(EEShard shard);

private:
    QNetworkRequest request(QUrl url, QByteArray method);
    QString getCommand(QNetworkReply *reply);
    void handleBucketsResponse(QByteArray method, QByteArray data, QNetworkReply *reply = nullptr);
    void handleFramesResponse(QByteArray method, QByteArray data, QNetworkReply *reply = nullptr);
    void handleShardsResponse(QByteArray method, QByteArray data, QNetworkReply *reply = nullptr);
    void handleUsersResponse(QByteArray method, QByteArray data, QNetworkReply *reply = nullptr);

private slots:
    void handleResponse(QNetworkReply *reply);

signals:
    //this signal is emmited when buckets list is received and parsed.
    void bucketsReceived(QList<EEBucket>);
    void filesForBucketReceived(QList<EEFile>);
    void bucketCreated(EEBucket);
    void bucketDeleted(QString id);
    void tokenCreated(EEToken);
    void shardsReceived(QList<EEShard>);
    void shardDownloaded(QByteArray);
    void shardAddedToFrame(EEShard);
    void userRegistered(EEUser);

    void requestError(EEError, QString method);

    //handling frames
    void frameCreated(EEFrame);

private:
    QNetworkAccessManager* mMgr;

    QString mEmail;
    QString mPassword;
};

#endif // EESDK_H
