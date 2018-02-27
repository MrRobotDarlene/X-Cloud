#ifndef EEFILE_H
#define EEFILE_H

#include "eehmac.h"

#include <QString>

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

    QString created() const;
    void setCreated(const QString &created);

    EEHMac hMac() const;
    void setHMac(const EEHMac &hMac);

private:
    QString mBucketId;
    QString mMimetype;
    QString mFilename;
    QString mFrame;
    int mSize;
    QString mId;
    QString mCreated;
    EEHMac mHMac;
};

#endif // EEFILE_H
