#ifndef EEBUCKETSDKPARSER_H
#define EEBUCKETSDKPARSER_H

#include <QObject>

class EEUser;
class EEToken;
class EEHMac;
class EEShard;
class EEFarmer;
class EEBucket;
class EEFile;
class EEFrame;
class EEError;
class EEShardRequest;

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

    static QList<EEFile> parseFiles(QByteArray data);
    static EEFile parseFile(QByteArray data);
    static EEHMac parseHMac(QByteArray data);

    static EEToken parseToken(QByteArray data);

    static QList<EEShard> parseShards(QByteArray data);
    static EEShard parseShard(QByteArray data);

    static EEFarmer parseFarmer(QByteArray data);

    static bool isError(QByteArray, EEError *);
    //frame parser
    static EEFrame parseFrame(QByteArray data);

    //forming json body for requests
    static QByteArray jsonCreateBucket(QString name, QStringList pubKeys = QStringList(""));
    static QByteArray jsonCreateToken(QString file, QString operation);
    static QByteArray jsonCreateUserRegister(QString email, QString password, QString pubkey = "", QString referalKey = "");

    static QByteArray jsonCreateShardRequest(EEShardRequest);


    static QDateTime correctDateTimeForm(QString string);
};

#endif // EEBUCKETSDKPARSER_H
