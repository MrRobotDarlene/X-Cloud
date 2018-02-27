#include "eeshardbasic.h"

EEShardBasic::EEShardBasic()
{

}

QString EEShardBasic::hash() const
{
    return mHash;
}

void EEShardBasic::setHash(const QString &hash)
{
    mHash = hash.toLower();
}

int EEShardBasic::size() const
{
    return mSize;
}

void EEShardBasic::setSize(int size)
{
    mSize = size;
}

int EEShardBasic::index() const
{
    return mIndex;
}

void EEShardBasic::setIndex(int index)
{
    mIndex = index;
}
