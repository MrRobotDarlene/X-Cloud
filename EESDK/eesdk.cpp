#include "eesdk.h"

#include "eesdkparser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCryptographicHash>

#define UNDEFINED   "-1"

#define BRIDGE "http://api.internxt.io:6382"

#define CMD_BUCKETS "buckets"
#define CMD_TOKENS  "tokens"
#define CMD_TOKEN   "token"
#define CMD_FRAMES  "frames"
#define CMD_FILES   "files"
#define CMD_SHARDS  "shards"
#define CMD_USERS   "users"

EESDK::EESDK(QObject *parent) : QObject(parent)
{
    mMgr = new QNetworkAccessManager();
    connect(mMgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleResponse(QNetworkReply*)));
}

EESDK::~EESDK()
{
    delete mMgr;
}

void EESDK::setCredentials(QString email, QString password)
{
    mEmail = email;
    mPassword = password;
}

QString EESDK::getEmail() const
{
    return mEmail;
}

QString EESDK::getPassword() const
{
    return mPassword;
}

void EESDK::registerUser(QString email, QString password, QString pubkey, QString referalKey)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_USERS));
    QNetworkRequest lRequest = request(lServiceURL, MTD_REGISTER_USER);
    QByteArray lBody = EESDKParser::jsonCreateUserRegister(email, password, pubkey, referalKey);

    mMgr->post(lRequest, lBody);
}

void EESDK::getBuckets()
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_BUCKETS));
    mMgr->get(request(lServiceURL, MTD_GET_BUCKETS));
}

void EESDK::getFilesFromBucket(QString bucket, QString startDate)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_BUCKETS).append("/").append(bucket).append("/").append(CMD_FILES));
    mMgr->get(request(lServiceURL, MTD_GET_FILES_FOR_BUCKETS));
}

void EESDK::getFileInfo(QString bucket, QString fileId, QString skip, QString limit, QString exclude)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_BUCKETS).append("/").append(bucket).append("/").append(CMD_FILES).append(fileId));

}

void EESDK::createBucket(QString name, QStringList pubKeys)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_BUCKETS));
    QByteArray lBody = EESDKParser::jsonCreateBucket(name, pubKeys);
    QNetworkRequest lRequest = request(lServiceURL, MTD_CREATE_BUCKET);

    mMgr->post(lRequest, lBody);
}

void EESDK::destroyBucket(QString id)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_BUCKETS).append("/").append(id));

    mMgr->deleteResource(request(lServiceURL, MTD_DESTROY_BUCKET));
}

void EESDK::createToken(QString id, QString file, QString operation)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_BUCKETS).append("/").append(id).append("/").append(CMD_TOKENS));
    QByteArray lBody = EESDKParser::jsonCreateToken(file, operation);
    QNetworkRequest lRequest = request(lServiceURL, MTD_CREATE_TOKEN);

    mMgr->post(lRequest, lBody);
}

void EESDK::getListOfShards(QString bucketId, QString fileId)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_BUCKETS).append("/").append(bucketId).append("/").append(CMD_FILES).append("/").append(fileId));
    mMgr->get(request(lServiceURL, MTD_GET_SHARDS));
}

void EESDK::addShardToFrame(EEShardRequest shard, QString frameId)
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_FRAMES).append("/").append(frameId));
    mMgr->put(request(lServiceURL, MTD_ADD_SHARDS_TO_FRAME), EESDKParser::jsonCreateShardRequest(shard));
}

void EESDK::createFrame()
{
    QUrl lServiceURL(QString(BRIDGE).append("/").append(CMD_FRAMES));

    mMgr->post(request(lServiceURL, MTD_CREATE_FRAME), QByteArray(""));
}

void EESDK::uploadShardToNode(EEShard shard, QByteArray data)
{
    QString lHost = QString("http://").append(shard.farmer().address().toString()).append(":").append(shard.farmer().port());
    QString lUrlString = QString(lHost).append("/").append(CMD_SHARDS).append("/").append(shard.hash()).append("?").append(CMD_TOKEN).append("=").append(shard.token());


    qDebug() << lUrlString;
    QUrl lServiceURL(lUrlString);
    QNetworkRequest lRequest(lServiceURL);
    lRequest.setRawHeader("content-type", "application/octet-stream");
    lRequest.setRawHeader("x-node-id", shard.farmer().nodeId().toUtf8());
    lRequest.setRawHeader(MTD_KEY, MTD_UPLOAD_SHARD);

    mMgr->post(lRequest, data);
}

void EESDK::downloadShardFromNode(EEShard shard)
{
    QString lHost = QString("http://").append(shard.farmer().address().toString()).append(":").append(shard.farmer().port());
    QString lUrlString = QString(lHost).append("/").append(CMD_SHARDS).append("/").append(shard.hash()).append("?").append(CMD_TOKEN).append("=").append(shard.token());

    QUrl lServiceURL(lUrlString);
    QNetworkRequest lRequest(lServiceURL);
    lRequest.setRawHeader("content-type", "application/octet-stream");
    lRequest.setRawHeader("x-node-id", shard.farmer().nodeId().toUtf8());

    lRequest.setRawHeader(MTD_KEY, MTD_DOWNLOD_SHARD);

    mMgr->get(lRequest);
}

QNetworkRequest EESDK::request(QUrl url, QByteArray method)
{
    QByteArray lAuth = mEmail.toUtf8();
    lAuth.append(":");
    QByteArray lPass = QCryptographicHash::hash(mPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
    lAuth.append(lPass);

    QNetworkRequest lRequest(url);
    lRequest.setRawHeader("Authorization", "Basic " + lAuth.toBase64());
    lRequest.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    lRequest.setRawHeader("Accept", "application/json");
    lRequest.setRawHeader(MTD_KEY, method);
    return lRequest;
}

QString EESDK::getCommand(QNetworkReply *reply)
{
    QString rCommand(UNDEFINED);
    if (!reply->request().url().path().isEmpty()) {
        rCommand = reply->request().url().path();
    }
    //remove /api/ if contains
    if (rCommand.contains(BRIDGE)) {
        int pos = rCommand.indexOf(BRIDGE);
        rCommand.remove(pos, QString(BRIDGE).size());
    }

    return rCommand;
}

void EESDK::handleBucketsResponse(QByteArray method, QByteArray data, QNetworkReply *)
{
    if (method == MTD_GET_BUCKETS) {
        emit bucketsReceived(EESDKParser::parseBucketList(data));
    } else if (method == MTD_CREATE_BUCKET) {
        emit bucketCreated(EESDKParser::parseBucket(data));
    } else if (method == MTD_DESTROY_BUCKET) {
        emit bucketDeleted(QString(data));
    } else if (method == MTD_CREATE_TOKEN) {
        emit tokenCreated(EESDKParser::parseToken(data));
    } else if (method == MTD_GET_SHARDS) {
        emit shardsReceived(EESDKParser::parseShards(data));
    } else if (method == MTD_GET_FILES_FOR_BUCKETS) {
        emit filesForBucketReceived(EESDKParser::parseFiles(data));
    }
}

void EESDK::handleFramesResponse(QByteArray method, QByteArray data, QNetworkReply *)
{
    if (method == MTD_CREATE_FRAME) {
        emit frameCreated(EESDKParser::parseFrame(data));
    } else if (method == MTD_ADD_SHARDS_TO_FRAME) {
        emit shardAddedToFrame(EESDKParser::parseShard(data));
    }
}

void EESDK::handleShardsResponse(QByteArray method, QByteArray data, QNetworkReply *)
{
    if (method == MTD_DOWNLOD_SHARD) {
        emit shardDownloaded(data);
    } else if (method == MTD_UPLOAD_SHARD) {
        qDebug() << MTD_UPLOAD_SHARD << ":" << data;
    }
}

void EESDK::handleUsersResponse(QByteArray method, QByteArray data, QNetworkReply *)
{
    if (method == MTD_REGISTER_USER) {
        emit userRegistered(EESDKParser::parseUser(data));
    }
}

void EESDK::handleResponse(QNetworkReply *reply)
{
    QByteArray lResponceData = reply->readAll();
    QByteArray lMethod = reply->request().rawHeader(MTD_KEY);
    EEError lError;

    qDebug() << "Responce: " << lResponceData;

    if (EESDKParser::isError(lResponceData, &lError)) {
        emit requestError(lError, lMethod);
    } else {
        if (getCommand(reply).contains(CMD_BUCKETS)) {
            handleBucketsResponse(lMethod, lResponceData, reply);
        } else if (getCommand(reply).contains(CMD_FRAMES)) {
            handleFramesResponse(lMethod, lResponceData, reply);
        } else if (getCommand(reply).contains(CMD_SHARDS)) {
            handleShardsResponse(lMethod, lResponceData, reply);
        } else if (getCommand(reply).contains(CMD_USERS)) {
            handleUsersResponse(lMethod, lResponceData, reply);
        }
    }
}
