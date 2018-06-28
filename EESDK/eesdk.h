#ifndef EESDK_H
#define EESDK_H

#include "EESDK/EEContainers/eecontainers.h"

#include <QObject>
#include <QNetworkRequest>
#include <QList>

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

    void setCredentials(const QString &login, const QString &password);

    QString getLogin() const;

    QString getPassword() const;

    QString getBridge();

    void authorizeUsingXToken(const QString &xtoken);

    void loginInWithCivic(const QString &civicToken);

    void registrateWithCivic(const QString &civicToken);
    //managing users
    void registerUser(const QString &email, const QString &password, const QString &pubkey = "", const QString &referalKey = "");

    //working with buckets
    void getBuckets();
    void getFilesFromBucket(const QString& bucketId);
    void getFileInfo(const QString &bucketId, const QString &fileId);
    void createBucket(const QString &name, QStringList pubKeys = QStringList(""));
    void destroyBucket(const QString &id);

    QString getBasicAuthorizationData() const;
    void setBasicAuthorizationData(const QString &basicAuthorizationData);

private:
    QNetworkRequest civicRequest(QUrl url, const QString &method, QString key, QString value);
    QNetworkRequest request(QUrl url, const QString &method);
    QString getCommand(QNetworkReply *reply);
    void handleBucketsResponse(QByteArray method, QByteArray data, QNetworkReply *reply = nullptr);
    void handleUsersResponse(QByteArray method, QByteArray data, QNetworkReply *reply = nullptr);
    void handleCivicResponse(QByteArray method, QByteArray data, QNetworkReply *reply = nullptr);

private slots:
    void handleResponse(QNetworkReply *reply);

signals:
    //this signal is emmited when buckets list is received and parsed.
    void bucketsReceived(QList<EEBucket*>);
    void filesForBucketReceived(QList<EEFile*>);
    void bucketCreated(EEBucket*);
    void bucketDeleted(QString id);
    void userRegistered(EEUser);
    void userLoggedInCivic(QString);
    void userLoggedInCivicUsingXData(QString);
    void userRegistratedCivicUsingXData(QString);

    void xtokenChecked(QString);
    void registrationTokenChecked(QString);

    void requestError(EEError, QString method);

public:
    static const QString mMtdKey;
    static const QString mMtdGetBuckets;
    static const QString mMtdGetShards;
    static const QString mMtdAddShardsToFrame;
    static const QString mMtdGetFilesForBuckets;
    static const QString mMtdCreateBucket;
    static const QString mMtdDestroyBucket;
    static const QString mMtdUploadShard;
    static const QString mMtdDownloadShard;
    static const QString mMtdFileInfo;

    static const QString mMtdRegisterUser;

    static const QString mMtdLoginInCivic;
    static const QString mMtdRegistrationCivic;
    static const QString mMtdAuthorization;

    static const QString mMtdDeleteFileFromBucket;

//#define BRIDGE  "https://api.storj.io"
    static const QString mBridgeAddress;

    static const QString mCommandBuckets;
    static const QString mCommandToken;
    static const QString mCommandFiles;
    static const QString mCommandUsers;
    static const QString mCommandInformation;
    static const QString mCommandXCloud;
private:
    QNetworkAccessManager* mMgr;

    QString mLogin;
    QString mPassword;
    QString mBasicAuthorizationData;
};

#endif // EESDK_H
