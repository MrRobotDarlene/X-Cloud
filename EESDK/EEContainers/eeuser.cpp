#include "eeuser.h"

EEUser::EEUser():
    mIsActivated()
{

}

QString EEUser::email() const
{
    return mEmail;
}

void EEUser::setEmail(const QString &email)
{
    mEmail = email;
}

QDateTime EEUser::created() const
{
    return mCreated;
}

void EEUser::setCreated(const QDateTime &created)
{
    mCreated = created;
}

bool EEUser::isActivated() const
{
    return mIsActivated;
}

void EEUser::setIsActivated(bool isActivated)
{
    mIsActivated = isActivated;
}
