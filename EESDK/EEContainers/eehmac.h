#ifndef HMAC_H
#define HMAC_H

#include <QString>

class EEHMac
{
public:
    EEHMac();

    QString value() const;
    void setValue(const QString &value);

    QString type() const;
    void setType(const QString &type);

private:
    QString mValue;
    QString mType;
};

#endif // HMAC_H
