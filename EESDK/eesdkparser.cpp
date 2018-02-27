#include "eesdkparser.h"
#include "EEContainers/eecontainers.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

//bucket keys
#define KEY_USER        "user"
#define KEY_ENCR_KEY    "encryptionKey"
#define KEY_PUB_PERMISS "publicPermissions"
#define KEY_CREATED     "created"
#define KEY_NAME        "name"
#define KEY_PUB_KEYS    "pubkeys"
#define KEY_STATUS      "status"
#define KEY_TRANSEFR    "transfer"
#define KEY_STORAGE     "storage"
#define KEY_ID          "id"

#define KEY_TOKEN       "token"
#define KEY_BUKET       "bucket"
#define KEY_EXIPES      "expires"
#define KEY_MIMETYPE    "mimetype"

//additional keys for frames
#define KEY_LOCKED      "locked"
#define KEY_SIZE        "size"
#define KEY_STORAGE_SIZE "storageSize"
#define KEY_SHARDS      "shards"
#define KEY_FILE        "file"
#define KEY_OPERATION   "operation"


//adding additional keys for shards
#define KEY_INDEX       "index"
#define KEY_HASH        "hash"
#define KEY_PARITY      "parity"
#define KEY_FARMER      "farmer"
#define KEY_CHALLENGE   "challenges"
#define KEY_TREE        "tree"
#define KEY_EXCLUDE     "exclude"

//additional keys for farmer
#define KEY_USER_AGENT  "userAgent"
#define KEY_PROTOCOL    "protocol"
#define KEY_ADDRESS     "address"
#define KEY_PORT        "port"
#define KEY_NODE_ID     "nodeID"
#define KEY_LATEST_SEEN "lastSeen"

//additional keys for files
#define KEY_FILE_NAME   "filename"

//additional keys for hmac
#define KEY_VALUE       "value"
#define KEY_TYPE        "type"
#define KEY_H_MAC       "hmac"

//additional keys for user registration
#define KEY_EMAIL       "email"
#define KEY_PASSWORD    "password"
#define KEY_PUB_KEY     "pubkey"
#define KEY_REFERAL     "referralPartner"
#define KEY_ACTIVED      "activated"

//additional key for error
#define KEY_ERROR       "error"

EESDKParser::EESDKParser(QObject *parent) : QObject(parent)
{

}

EEUser EESDKParser::parseUser(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEUser rUser;

    rUser.setEmail(lObj.value(KEY_EMAIL).toString());
    rUser.setCreated(lObj.value(KEY_CREATED).toString());
    rUser.setIsActivated(lObj.value(KEY_ACTIVED).toBool());

    return rUser;
}

QList<EEBucket> EESDKParser::parseBucketList(QByteArray data)
{
    QList<EEBucket> rBuckets;

    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonArray lBuckets = lDoc.array();

    if (lBuckets.count() > 0) {
        foreach (QJsonValue bucket, lBuckets) {
            QByteArray lTmp = QJsonDocument(bucket.toObject()).toJson();
            rBuckets.append(EESDKParser::parseBucket(lTmp));
        }
    }

    return rBuckets;
}

EEBucket EESDKParser::parseBucket(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEBucket lTmpBucket;

    lTmpBucket.setUser(lObj.value(KEY_USER).toString());
    lTmpBucket.setEncryptionKey(lObj.value(KEY_ENCR_KEY).toString());
    foreach (QJsonValue pubPermiss, lObj.value(KEY_PUB_PERMISS).toArray()) {
        lTmpBucket.appendPublicPermissions(pubPermiss.toString());
    }
    lTmpBucket.setCreated(lObj.value(KEY_CREATED).toString());
    lTmpBucket.setName(lObj.value(KEY_NAME).toString());
    foreach (QJsonValue pubKey, lObj.value(KEY_PUB_KEYS).toArray()) {
        lTmpBucket.appendPubKey(pubKey.toString());
    }
    lTmpBucket.setIsActive(lObj.value(KEY_STATUS).toBool());
    lTmpBucket.setTransfer(lObj.value(KEY_TRANSEFR).toInt());
    lTmpBucket.setStorage(lObj.value(KEY_STORAGE).toInt());
    lTmpBucket.setId(lObj.value(KEY_ID).toString());

    return lTmpBucket;
}

QList<EEFile> EESDKParser::parseFiles(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonArray lArray = lDoc.array();
    QList<EEFile>rFiles;

    foreach (QJsonValue value, lArray) {
        rFiles.append(EESDKParser::parseFile(QJsonDocument(value.toObject()).toJson()));
    }

    return rFiles;
}

EEFile EESDKParser::parseFile(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEFile rFile;

    rFile.setBucketId(lObj.value(KEY_BUKET).toString());
    rFile.setMimetype(lObj.value(KEY_MIMETYPE).toString());
    rFile.setFilename(lObj.value(KEY_FILE_NAME).toString());
    rFile.setSize(lObj.value(KEY_SIZE).toInt());
    rFile.setId(lObj.value(KEY_ID).toString());
    rFile.setCreated(lObj.value(KEY_CREATED).toString());
    rFile.setHMac(parseHMac(lObj.value(KEY_H_MAC).toString().toUtf8()));

    return rFile;
}

EEHMac EESDKParser::parseHMac(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEHMac rHMac;

    rHMac.setValue(lObj.value(KEY_VALUE).toString());
    rHMac.setType(lObj.value(KEY_TYPE).toString());

    return rHMac;
}

EEToken EESDKParser::parseToken(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEToken rToken;

    rToken.setToken(lObj.value(KEY_TOKEN).toString());
    rToken.setBucket(lObj.value(KEY_BUKET).toString());
    rToken.setExpires(lObj.value(KEY_EXIPES).toString());
    rToken.setOperation(lObj.value(KEY_OPERATION).toString());
    rToken.setEncryptionKey(lObj.value(KEY_ENCR_KEY).toString());
    rToken.setSize(lObj.value(KEY_SIZE).toInt());
    rToken.setMimetype(lObj.value(KEY_MIMETYPE).toString());

    return rToken;
}

QList<EEShard> EESDKParser::parseShards(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonArray lArray = lDoc.array();
    QList<EEShard> rShards;

    foreach (QJsonValue value, lArray) {
        rShards.append(parseShard(QJsonDocument(value.toObject()).toJson()));
    }

    return rShards;
}

EEShard EESDKParser::parseShard(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEShard rShard;

    rShard.setIndex(lObj.value(KEY_INDEX).toInt());
    rShard.setHash(lObj.value(KEY_HASH).toString());
    rShard.setSize(lObj.value(KEY_SIZE).toInt());
    rShard.setIsPartial(lObj.value(KEY_PARITY).toBool());
    rShard.setToken(lObj.value(KEY_TOKEN).toString());

    rShard.setFarmer(EESDKParser::parseFarmer(QJsonDocument(lObj.value(KEY_FARMER).toObject()).toJson()));

    rShard.setOperation(lObj.value(KEY_OPERATION).toString());

    return rShard;
}

EEFarmer EESDKParser::parseFarmer(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEFarmer rFarmer;

    rFarmer.setUserAgent(lObj.value(KEY_USER_AGENT).toString());
    rFarmer.setProtocol(lObj.value(KEY_PROTOCOL).toString());
    rFarmer.setPort(QString().number(lObj.value(KEY_PORT).toInt()));
    rFarmer.setAddress(lObj.value(KEY_ADDRESS).toString());
    rFarmer.setNodeId(lObj.value(KEY_NODE_ID).toString());
    rFarmer.setLastSeen(lObj.value(KEY_LATEST_SEEN).toString());

    return rFarmer;
}

bool EESDKParser::isError(QByteArray data, EEError *err)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    bool rError = false;

    if (lObj.contains(KEY_ERROR)) {
        rError = true;
        err->setMessage(lObj.value(KEY_ERROR).toString());
    }

    return rError;
}

EEFrame EESDKParser::parseFrame(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEFrame rFrame;

    rFrame.setUser(lObj.value(KEY_USER).toString());
    foreach (QJsonValue shard, lObj.value(KEY_SHARDS).toArray()) {
        rFrame.appendShard(shard.toString());
    }
    rFrame.setStorageSize(lObj.value(KEY_STORAGE_SIZE).toInt());
    rFrame.setSize(lObj.value(KEY_SIZE).toInt());
    rFrame.setIsActive(!lObj.value(KEY_LOCKED).toBool());
    rFrame.setCreated(lObj.value(KEY_CREATED).toString());
    rFrame.setId(lObj.value(KEY_ID).toString());

    return rFrame;
}

QByteArray EESDKParser::jsonCreateBucket(QString name, QStringList pubKeys)
{
    QJsonObject rJson;
    QJsonArray lPubKeysArray;

    foreach (QString pubKey, pubKeys) {
        if (!pubKey.isEmpty()) {
            lPubKeysArray.append(pubKey);
        }
    }

    rJson.insert(KEY_PUB_KEYS, lPubKeysArray);
    rJson.insert(KEY_NAME, name);

    return QJsonDocument(rJson).toJson();
}

QByteArray EESDKParser::jsonCreateToken(QString file, QString operation)
{
    QJsonObject rJson;

    rJson.insert(KEY_OPERATION, operation);
    rJson.insert(KEY_FILE, file);

    return QJsonDocument(rJson).toJson();
}

QByteArray EESDKParser::jsonCreateUserRegister(QString email, QString password, QString pubkey, QString referalKey)
{
    QJsonObject rJson;

    rJson.insert(KEY_EMAIL, email);
    rJson.insert(KEY_PASSWORD, password);
    rJson.insert(KEY_PUB_KEY, pubkey);
    rJson.insert(KEY_REFERAL, referalKey);

    return QJsonDocument(rJson).toJson();
}

QByteArray EESDKParser::jsonCreateShardRequest(EEShardRequest s)
{
    QJsonObject rJson;
    QJsonArray lChallenges;
    QJsonArray lTree;
    QJsonArray lExclude;

    foreach (QString ch, s.challenges()) {
        lChallenges.append(ch);
    }

    foreach (QString tree, s.tree()) {
        lTree.append(tree);
    }

    foreach (QString ex, s.exclude()) {
        lExclude.append(ex);
    }

    rJson.insert(KEY_HASH, s.hash());
    rJson.insert(KEY_INDEX, s.index());
    rJson.insert(KEY_SIZE, s.size());
    rJson.insert(KEY_CHALLENGE, lChallenges);
    rJson.insert(KEY_TREE, lTree);
    rJson.insert(KEY_EXCLUDE, lExclude);


    return QJsonDocument(rJson).toJson();
}
