#ifndef EEFRAME_H
#define EEFRAME_H

#include "eeabstractcontainer.h"

#include <QStringList>

class EEFrame : public EEAbstractContainer
{
public:
    explicit EEFrame();
    EEFrame(EEFrame*p);

    int size() const;
    void setSize(int size);

    QStringList shards() const;
    void appendShard(QString shard);

private:
    QStringList mShards;
    int mSize;
};

#endif // EEFRAME_H
