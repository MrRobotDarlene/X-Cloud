#include "eesdkparser.h"
#include "eecontainers.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QString>
#include <QDateTime>

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

#define KEY_SIZE        "size"

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

//xtoken
#define KEY_MESSAGE "message"

//public key
#define KEY_PUBLICKEY "publicKey"

EESDKParser::EESDKParser(QObject *parent) : QObject(parent)
{

}

EEUser EESDKParser::parseUser(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEUser rUser;

    rUser.setEmail(lObj.value(KEY_EMAIL).toString());

    rUser.setCreated(correctDateTimeForm(lObj.value(KEY_CREATED).toString()));
    rUser.setIsActivated(lObj.value(KEY_ACTIVED).toBool());

    return rUser;
}

QList<EEBucket*> EESDKParser::parseBucketList(QByteArray data)
{
    QList<EEBucket*> rBuckets;

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

EEBucket* EESDKParser::parseBucket(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEBucket *lTmpBucket = new EEBucket();

    lTmpBucket->setUser(lObj.value(KEY_USER).toString());
    lTmpBucket->setEncryptionKey(lObj.value(KEY_ENCR_KEY).toString());
    foreach (QJsonValue pubPermiss, lObj.value(KEY_PUB_PERMISS).toArray()) {
        lTmpBucket->appendPublicPermissions(pubPermiss.toString());
    }

    lTmpBucket->setCreated(correctDateTimeForm(lObj.value(KEY_CREATED).toString()));
    lTmpBucket->setName(lObj.value(KEY_NAME).toString());
    foreach (QJsonValue pubKey, lObj.value(KEY_PUB_KEYS).toArray()) {
        lTmpBucket->appendPubKey(pubKey.toString());
    }
    lTmpBucket->setIsActive(lObj.value(KEY_STATUS).toBool());
    lTmpBucket->setTransfer(lObj.value(KEY_TRANSEFR).toInt());
    lTmpBucket->setStorage(lObj.value(KEY_STORAGE).toInt());
    lTmpBucket->setId(lObj.value(KEY_ID).toString());

    return lTmpBucket;
}

QList<EEFile*> EESDKParser::parseFiles(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonArray lArray = lDoc.array();
    QList<EEFile*>lFiles;

    foreach (QJsonValue value, lArray) {
        lFiles.append(EESDKParser::parseFile(QJsonDocument(value.toObject()).toJson()));
    }

    return lFiles;
}

EEFile *EESDKParser::parseFile(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();
    EEFile *lFile = new EEFile;

    lFile->setBucketId(lObj.value(KEY_BUKET).toString());
    lFile->setMimetype(lObj.value(KEY_MIMETYPE).toString());
    lFile->setFilename(lObj.value(KEY_FILE_NAME).toString());
    lFile->setSize(lObj.value(KEY_SIZE).toInt());
    lFile->setId(lObj.value(KEY_ID).toString());
    lFile->setCreated(correctDateTimeForm(lObj.value(KEY_CREATED).toString()));
    lFile->setHMac(parseHMac(lObj.value(KEY_H_MAC).toString().toUtf8()));

    return lFile;
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

QByteArray EESDKParser::jsonCreateBucket(const QString &name, QStringList pubKeys)
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

QByteArray EESDKParser::jsonCreateUserRegister(const QString &email, const QString &password, const QString &pubkey, const QString &referalKey)
{
    QJsonObject rJson;

    rJson.insert(KEY_EMAIL, email);
    rJson.insert(KEY_PASSWORD, password);
    rJson.insert(KEY_PUB_KEY, pubkey);
    rJson.insert(KEY_REFERAL, referalKey);

    return QJsonDocument(rJson).toJson();
}

QString EESDKParser::parsePublicKey(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();

    QString lMessage = lObj.value(KEY_PUBLICKEY).toString();
    return lMessage;
}

QString EESDKParser::parseXTokenResponse(QByteArray data)
{
    QJsonDocument lDoc = QJsonDocument::fromJson(data);
    QJsonObject lObj = lDoc.object();

    QString lMessage = lObj.value(KEY_MESSAGE).toString();
    return lMessage;
}

QDateTime EESDKParser::correctDateTimeForm(const QString &string)
{
    QDateTime lCorrectDateTime = QDateTime::fromString(string, "yyyy-MM-ddThh:mm:ss.zZ");
    lCorrectDateTime.setTimeSpec(Qt::UTC);
    return lCorrectDateTime;
}

