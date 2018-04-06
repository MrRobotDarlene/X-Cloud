#include "eeabstractcontainer.h"

#include <QDebug>

EEAbstractContainer::EEAbstractContainer()
{
    mUser = "";
    mIsActive = -1;
    mStorageSize = -1;
    mCreated;
    mId = "";
}

EEAbstractContainer::EEAbstractContainer(EEAbstractContainer *p)
{
    mUser = p->user();
    mIsActive = p->isActive();
    mStorageSize = p->storageSize();
    mCreated = p->created();
    mId = p->id();
}

QString EEAbstractContainer::user() const
{
    return mUser;
}

void EEAbstractContainer::setUser(const QString &user)
{
    mUser = user;
}

bool EEAbstractContainer::isActive() const
{
    return mIsActive;
}

void EEAbstractContainer::setIsActive(bool isActive)
{
    mIsActive = isActive;
}

QDateTime EEAbstractContainer::created() const
{
    return mCreated;
}

void EEAbstractContainer::setCreated(const QDateTime &created)
{
    mCreated = created;
}

QString EEAbstractContainer::id() const
{
    return mId;
}

void EEAbstractContainer::setId(const QString &id)
{
    mId = id;
}

int EEAbstractContainer::storageSize() const
{
    return mStorageSize;
}

void EEAbstractContainer::setStorageSize(int storageSize)
{
    mStorageSize = storageSize;
}
