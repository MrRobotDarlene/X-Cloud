#include "eesdk.h"

#include "eesdkparser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCryptographicHash>


const QString EESDK::mMtdKey("Called method");
const QString EESDK::mMtdGetBuckets("Get list of buckets");
const QString EESDK::mMtdGetFilesForBuckets("Get files list of buckets");
const QString EESDK::mMtdCreateBucket("Create new bucket");
const QString EESDK::mMtdDestroyBucket("Destroy bucket for given id");
const QString EESDK::mMtdFileInfo("Get file metadata");

const QString EESDK::mMtdRegisterUser("Register new user");

const QString EESDK::mMtdLoginInCivic("Login in user");
const QString EESDK::mMtdRegistrationCivic("Registration in user");
const QString EESDK::mMtdAuthorization("Authorize using xtoken");

const QString EESDK::mMtdDeleteFileFromBucket("Delete file");

//#define BRIDGE  "https://api.storj.io"
const QString EESDK::mBridgeAddress("http://api.internxt.com:6382");

const QString EESDK::mCommandBuckets("buckets");
const QString EESDK::mCommandToken("token");
const QString EESDK::mCommandFiles("files");
const QString EESDK::mCommandUsers("users");
const QString EESDK::mCommandInformation("info");
const QString EESDK::mCommandXCloud("xcloud");

#define UNDEFINED   "-1"

EESDK::EESDK(QObject *parent) : QObject(parent)
{
    mMgr = new QNetworkAccessManager();
    connect(mMgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleResponse(QNetworkReply*)));
}

EESDK::~EESDK()
{
    delete mMgr;
}

void EESDK::setCredentials(const QString& login,const  QString& password)
{
    mLogin = login;
    mPassword = password;
}

void EESDK::setBasicAuthorizationData(const QString &basicAuthorizationData)
{
    mBasicAuthorizationData = basicAuthorizationData;
}

QString EESDK::getBasicAuthorizationData() const
{
    return mBasicAuthorizationData;
}

QString EESDK::getLogin() const
{
    return mLogin;
}

QString EESDK::getPassword() const
{
    return mPassword;
}

QString EESDK::getBridge()
{
    return mBridgeAddress;
}

void EESDK::authorizeUsingXToken(const QString &xtoken)
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/").append(mCommandXCloud));

    mMgr->get(civicRequest(lServiceURL, mMtdAuthorization, "authorization", xtoken));
}

void EESDK::loginInWithCivic(const QString &civicToken)
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/").append(mCommandXCloud).append("/").append(mCommandToken));

    mMgr->get(civicRequest(lServiceURL, mMtdLoginInCivic, "civic", civicToken));
}

void EESDK::registrateWithCivic(const QString &civicToken)
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/").append(mCommandUsers));
    QNetworkRequest lRequest = civicRequest(lServiceURL, mMtdRegistrationCivic, "civic", civicToken);
    QByteArray lBody = "";

    mMgr->post(lRequest, lBody);
}

void EESDK::registerUser(const QString &email, const  QString &password, const QString &pubkey, const QString &referalKey)
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/").append(mCommandUsers));
    QNetworkRequest lRequest = request(lServiceURL, mMtdRegisterUser);
    QByteArray lBody = EESDKParser::jsonCreateUserRegister(email, password, pubkey, referalKey);

    mMgr->post(lRequest, lBody);
}

void EESDK::getBuckets()
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/").append(mCommandBuckets));
    mMgr->get(request(lServiceURL, mMtdGetBuckets));
}

void EESDK::getFilesFromBucket(const QString &bucketId)
{
    QUrl lServiceURL(QString(mBridgeAddress)
                     .append("/")
                     .append(mCommandBuckets)
                     .append("/")
                     .append(bucketId)
                     .append("/")
                     .append(mCommandFiles));

    mMgr->get(request(lServiceURL, mMtdGetFilesForBuckets));
}

void EESDK::getFileInfo(const QString &bucketId, const QString &fileId)
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/")
                     .append(mCommandBuckets).append("/")
                     .append(bucketId).append("/")
                     .append(mCommandFiles)
                     .append(fileId)
                     .append("/")
                     .append(mCommandInformation));

    mMgr->get(request(lServiceURL, mMtdFileInfo));
}

void EESDK::createBucket(const QString &name, QStringList pubKeys)
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/").append(mCommandBuckets));
    QByteArray lBody = EESDKParser::jsonCreateBucket(name, pubKeys);
    QNetworkRequest lRequest = request(lServiceURL, mMtdCreateBucket);

    mMgr->post(lRequest, lBody);
}

void EESDK::destroyBucket(const QString &id)
{
    QUrl lServiceURL(QString(mBridgeAddress).append("/").append(mCommandBuckets).append("/").append(id));

    mMgr->deleteResource(request(lServiceURL, mMtdDestroyBucket));
}

QNetworkRequest EESDK::civicRequest(QUrl url, const QString & method, QString key, QString value)
{
    QNetworkRequest lRequest(url);
    lRequest.setRawHeader(key.toUtf8(), value.toUtf8());
    lRequest.setRawHeader("Content-Type", "application/json; charset=utf-8");
    lRequest.setRawHeader(mMtdKey.toLatin1(), method.toLatin1());

    return lRequest;
}

QNetworkRequest EESDK::request(QUrl url, const QString &method)
{
    QByteArray lAuth = mLogin.toUtf8();
    lAuth.append(":");
    QByteArray lPass = QCryptographicHash::hash(mPassword.toUtf8(), QCryptographicHash::Sha256).toHex();
    lAuth.append(lPass);

    QNetworkRequest lRequest(url);
    lRequest.setRawHeader("Authorization", "Basic " + lAuth.toBase64());// mBasicAuthorizationData.toUtf8());
    lRequest.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    lRequest.setRawHeader("Accept", "application/json");
    lRequest.setRawHeader(mMtdKey.toLatin1(), method.toLatin1());
    return lRequest;
}

QString EESDK::getCommand(QNetworkReply *reply)
{
    QString rCommand(UNDEFINED);
    if (!reply->request().url().path().isEmpty()) {
        rCommand = reply->request().url().path();
    }
    //remove /api/ if contains
    if (rCommand.contains(mBridgeAddress)) {
        int pos = rCommand.indexOf(mBridgeAddress);
        rCommand.remove(pos, QString(mBridgeAddress).size());
    }

    return rCommand;
}

void EESDK::handleBucketsResponse(QByteArray method, QByteArray data, QNetworkReply *)
{
    if (method == mMtdGetBuckets) {
        emit bucketsReceived(EESDKParser::parseBucketList(data));
    } else if (method == mMtdCreateBucket) {
        emit bucketCreated(EESDKParser::parseBucket(data));
    } else if (method == mMtdDestroyBucket) {
        emit bucketDeleted(QString(data));
    } else if (method == mMtdGetFilesForBuckets) {
        emit filesForBucketReceived(EESDKParser::parseFiles(data));
    }
}

void EESDK::handleUsersResponse(QByteArray method, QByteArray data, QNetworkReply *reply)
{
    if (method == mMtdRegisterUser) {
        emit userRegistered(EESDKParser::parseUser(data));
    } else if (method == mMtdRegistrationCivic) {
#warning credentials suppose to be removed from header, so it has to be reworked somehow
        QString lAuthorizationData = reply->rawHeader(QByteArray("x-user-data-email")) + ":" +  reply->rawHeader(QByteArray("x-user-data-phone"));
        emit userRegistratedCivicUsingXData(lAuthorizationData);
        //emit registrationTokenChecked(EESDKParser::parseXTokenResponse(data));
    }
}

void EESDK::handleCivicResponse(QByteArray method, QByteArray data, QNetworkReply *reply)
{
    if (method == mMtdLoginInCivic) {
#warning maybe replace here
        //emit userLoggedInCivic(reply->rawHeader(QByteArray("x-token")));
        QString lAuthorizationData = reply->rawHeader(QByteArray("x-user-data-email")) + ":" +  reply->rawHeader(QByteArray("x-user-data-phone"));
        emit userLoggedInCivicUsingXData(lAuthorizationData);

#warning rework signal-slot connection with two string x-token, public key
        //emit userLoggedInCivic(reply->rawHeader(QByteArray("x-token"), parsePublicKey(data)));
    } else if (method == mMtdAuthorization) {
        emit xtokenChecked(EESDKParser::parseXTokenResponse(data));
    }
}

void EESDK::handleResponse(QNetworkReply *reply)
{
    QByteArray lResponseData = reply->readAll();
    QByteArray lErrorData = reply->errorString().toLatin1();
    QByteArray lMethod = reply->request().rawHeader(mMtdKey.toLatin1());
    EEError lError;

    qDebug() << reply->rawHeaderPairs();
    qDebug() << "Responce: " << lResponseData;

    if (!lErrorData.isEmpty()) {
        qDebug() << "Error" << lErrorData;
    }

    if (EESDKParser::isError(lResponseData, &lError)) {
        emit requestError(lError, lMethod);
    } else {
        if (getCommand(reply).contains(mCommandBuckets)) {
            handleBucketsResponse(lMethod, lResponseData, reply);
        } else if (getCommand(reply).contains(mCommandUsers)) {
            handleUsersResponse(lMethod, lResponseData, reply);
        } else if (getCommand(reply).contains(mCommandXCloud)) {
            handleCivicResponse(lMethod, lResponseData, reply);
        }
    }

    //reply->deleteLater();
}
