#include "eeerror.h"

EEError::EEError()
{

}

QString EEError::message() const
{
    return mMessage;
}

void EEError::setMessage(const QString &message)
{
    mMessage = message;
}
