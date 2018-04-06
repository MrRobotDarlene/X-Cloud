#ifndef EEUSER_H
#define EEUSER_H

#include <QString>
#include <QDateTime>

class EEUser
{
public:
    EEUser();

    QString email() const;
    void setEmail(const QString &email);

    QDateTime created() const;
    void setCreated(const QDateTime &created);

    bool isActivated() const;
    void setIsActivated(bool isActivated);

private:
    QString mEmail;
    QDateTime mCreated;
    bool mIsActivated;
};

#endif // EEUSER_H
