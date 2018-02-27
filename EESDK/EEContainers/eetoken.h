#ifndef EETOKEN_H
#define EETOKEN_H

#include <QString>

class EEToken
{
public:
    EEToken();

    static const QString OperationPull;
    static const QString OperationPush;

    QString token() const;
    void setToken(const QString &token);

    QString bucket() const;
    void setBucket(const QString &bucket);

    QString expires() const;
    void setExpires(const QString &expires);

    QString operation() const;
    void setOperation(const QString &operation);

    QString encryptionKey() const;
    void setEncryptionKey(const QString &encryptionKey);

    int size() const;
    void setSize(int size);

    QString mimetype() const;
    void setMimetype(const QString &mimetype);

private:
    QString mToken;
    QString mBucket;
    QString mExpires;
    QString mOperation;
    QString mEncryptionKey;
    int mSize;
    QString mMimetype;
};

#endif // EETOKEN_H
