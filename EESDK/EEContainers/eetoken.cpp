#include "eetoken.h"

const QString EEToken::OperationPull = "PULL";
const QString EEToken::OperationPush = "PUSH";

EEToken::EEToken()
{

}

QString EEToken::token() const
{
    return mToken;
}

void EEToken::setToken(const QString &token)
{
    mToken = token;
}

QString EEToken::bucket() const
{
    return mBucket;
}

void EEToken::setBucket(const QString &bucket)
{
    mBucket = bucket;
}

QString EEToken::expires() const
{
    return mExpires;
}

void EEToken::setExpires(const QString &expires)
{
    mExpires = expires;
}

QString EEToken::operation() const
{
    return mOperation;
}

void EEToken::setOperation(const QString &operation)
{
    mOperation = operation;
}

QString EEToken::encryptionKey() const
{
    return mEncryptionKey;
}

void EEToken::setEncryptionKey(const QString &encryptionKey)
{
    mEncryptionKey = encryptionKey;
}

int EEToken::size() const
{
    return mSize;
}

void EEToken::setSize(int size)
{
    mSize = size;
}

QString EEToken::mimetype() const
{
    return mMimetype;
}

void EEToken::setMimetype(const QString &mimetype)
{
    mMimetype = mimetype;
}
