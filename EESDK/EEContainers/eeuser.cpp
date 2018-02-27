#include "eeuser.h"

EEUser::EEUser()
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

QString EEUser::created() const
{
    return mCreated;
}

void EEUser::setCreated(const QString &created)
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
