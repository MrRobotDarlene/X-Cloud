#ifndef EEBUCKET_H
#define EEBUCKET_H

#include "eeabstractcontainer.h"

#include <QString>
#include <QStringList>

class EEBucket : public EEAbstractContainer
{
public:
    EEBucket();
    EEBucket(EEBucket *b);

    QString encryptionKey() const;
    void setEncryptionKey(const QString &encryptionKey);

    QStringList publicPermissions() const;
    void appendPublicPermissions(const QString publicPermissions);

    QStringList pubKeys() const;
    void appendPubKey(QString pubKey);

    int transfer() const;
    void setTransfer(int transfer);

    int storage() const;
    void setStorage(int storage);

    QString name() const;
    void setName(const QString &name);

    inline bool operator ==(EEBucket &bucket) {
        return (bucket.mName == this->mName
                && bucket.mEncryptionKey == this->mEncryptionKey
                && bucket.mPublicPermissions == this->mPublicPermissions
                && bucket.mPubKeys == this->mPubKeys);
    }

private:
    QString mName = "";
    QString mEncryptionKey = "";
    QStringList mPublicPermissions;
    QStringList mPubKeys;
    int mTransfer = -1;
    int mStorage = -1;
};

#endif // EEBUCKET_H
