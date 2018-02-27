#include "eeframe.h"


EEFrame::EEFrame()
{
}

EEFrame::EEFrame(EEFrame *p) : EEAbstractContainer(p),
    mSize{p->size()},
    mShards{QStringList(p->shards())}
{
}

int EEFrame::size() const
{
    return mSize;
}

void EEFrame::setSize(int size)
{
    mSize = size;
}

QStringList EEFrame::shards() const
{
    return mShards;
}

void EEFrame::appendShard(QString shard)
{
    mShards.append(shard);
}
