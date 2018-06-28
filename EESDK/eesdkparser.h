#ifndef EEBUCKETSDKPARSER_H
#define EEBUCKETSDKPARSER_H

#include <QObject>

class EEUser;
class EEHMac;
class EEFarmer;
class EEBucket;
class EEFile;
class EEError;

class QDateTime;
class QString;

class EESDKParser : public QObject
{
    Q_OBJECT
public:
    explicit EESDKParser(QObject *parent = nullptr);

    //parsing response from server

    static EEUser parseUser(QByteArray data);
    //buckets
    static QList<EEBucket *> parseBucketList(QByteArray data);
    static EEBucket *parseBucket(QByteArray data);

    //files
    static QList<EEFile *> parseFiles(QByteArray data);
    static EEFile* parseFile(QByteArray data);
    static EEHMac parseHMac(QByteArray data);

    static bool isError(QByteArray, EEError *);

    //forming json body for requests
    static QByteArray jsonCreateBucket(const QString &name, QStringList pubKeys = QStringList(""));
    static QByteArray jsonCreateUserRegister(const QString &email, const QString &password, const QString &pubkey = "", const QString &referalKey = "");


    static QString parsePublicKey(QByteArray data);
    static QString parseXTokenResponse(QByteArray data);

    static QDateTime correctDateTimeForm(const QString &string);
};

#endif // EEBUCKETSDKPARSER_H
