#ifndef EEFILE_H
#define EEFILE_H

#include "eehmac.h"

#include <QString>
#include <QDateTime>

class EEFile
{
public:
    EEFile();

    QString bucketId() const;
    void setBucketId(const QString &bucketId);

    QString mimetype() const;
    void setMimetype(const QString &mimetype);

    QString filename() const;
    void setFilename(const QString &filename);

    QString frame() const;
    void setFrame(const QString &frame);

    int size() const;
    void setSize(int size);

    QString id() const;
    void setId(const QString &id);

    QDateTime created() const;
    void setCreated(const QDateTime &created);

    EEHMac hMac() const;
    void setHMac(const EEHMac &hMac);

    inline bool operator ==(EEFile file) {
        return (this->mBucketId == file.mBucketId
                && this->mMimetype == file.mMimetype
                && this->mFilename == file.mFilename
                && this->mFrame == file.mFrame
                && this->mSize == file.mSize
                && this->mId == file.mId
                && this->mCreated == file.mCreated);
    }

private:
    QString mBucketId;
    QString mMimetype;
    QString mFilename;
    QString mFrame;
    int mSize;
    QString mId;
    QDateTime mCreated;
    EEHMac mHMac;
};

#endif // EEFILE_H
