#include "eehmac.h"

EEHMac::EEHMac()
{

}

QString EEHMac::value() const
{
    return mValue;
}

void EEHMac::setValue(const QString &value)
{
    mValue = value;
}

QString EEHMac::type() const
{
    return mType;
}

void EEHMac::setType(const QString &type)
{
    mType = type;
}
