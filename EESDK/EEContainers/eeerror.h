#ifndef EEERROR_H
#define EEERROR_H

#include <QString>

class EEError
{
public:
    EEError();

    QString message() const;
    void setMessage(const QString &message);

private:
    QString mMessage;
};

#endif // EEERROR_H
