#include "eebucket.h"

#include <QDebug>

EEBucket::EEBucket()
{
    mName = "";
    mEncryptionKey = "";
    mTransfer = -1;
    mStorage = -1;
}

EEBucket::EEBucket(EEBucket *b) : EEAbstractContainer(b)
{
    mName = b->name();
    mEncryptionKey = b->encryptionKey();
    mPublicPermissions = QStringList(b->publicPermissions());
    mPubKeys = QStringList(b->pubKeys());
    mTransfer = b->transfer();
    mStorage = b->storage();
}

QString EEBucket::encryptionKey() const
{
    return mEncryptionKey;
}

void EEBucket::setEncryptionKey(const QString &encryptionKey)
{
    mEncryptionKey = encryptionKey;
}

QStringList EEBucket::publicPermissions() const
{
    return mPublicPermissions;
}

void EEBucket::appendPublicPermissions(const QString publicPermissions)
{
    mPublicPermissions.append(publicPermissions);
}

QStringList EEBucket::pubKeys() const
{
    return mPubKeys;
}

void EEBucket::appendPubKey(QString pubKey)
{
    mPubKeys.append(pubKey);
}

int EEBucket::transfer() const
{
    return mTransfer;
}

void EEBucket::setTransfer(int transfer)
{
    mTransfer = transfer;
}

int EEBucket::storage() const
{
    return mStorage;
}

void EEBucket::setStorage(int storage)
{
    mStorage = storage;
}

QString EEBucket::name() const
{
    return mName;
}

void EEBucket::setName(const QString &name)
{
    mName = name;
}
