#include "eefile.h"

EEFile::EEFile():
    mBucketId{""},
    mMimetype{""},
    mFilename{""},
    mFrame{""},
    mSize{-1},
    mId{""},
    mHMac()
{
}

QString EEFile::bucketId() const
{
    return mBucketId;
}

void EEFile::setBucketId(const QString &bucketId)
{
    mBucketId = bucketId;
}

QString EEFile::mimetype() const
{
    return mMimetype;
}

void EEFile::setMimetype(const QString &mimetype)
{
    mMimetype = mimetype;
}

QString EEFile::filename() const
{
    return mFilename;
}

void EEFile::setFilename(const QString &filename)
{
    mFilename = filename;
}

QString EEFile::frame() const
{
    return mFrame;
}

void EEFile::setFrame(const QString &frame)
{
    mFrame = frame;
}

int EEFile::size() const
{
    return mSize;
}

void EEFile::setSize(int size)
{
    mSize = size;
}

QString EEFile::id() const
{
    return mId;
}

void EEFile::setId(const QString &id)
{
    mId = id;
}

QDateTime EEFile::created() const
{
    return mCreated;
}

void EEFile::setCreated(const QDateTime &created)
{
    mCreated = created;
}

EEHMac EEFile::hMac() const
{
    return mHMac;
}

void EEFile::setHMac(const EEHMac &hMac)
{
    mHMac = hMac;
}

