#include "eeshard.h"

EEShard::EEShard():
    mIsPartial{false},
    mToken{""},
    mOperation{""}

{

}

bool EEShard::isPartial() const
{
    return mIsPartial;
}

void EEShard::setIsPartial(bool isPartial)
{
    mIsPartial = isPartial;
}

QString EEShard::token() const
{
    return mToken;
}

void EEShard::setToken(const QString &token)
{
    mToken = token;
}

QString EEShard::operation() const
{
    return mOperation;
}

void EEShard::setOperation(const QString &operation)
{
    mOperation = operation;
}

EEFarmer EEShard::farmer() const
{
    return mFarmer;
}

void EEShard::setFarmer(const EEFarmer &farmer)
{
    mFarmer = farmer;
}
