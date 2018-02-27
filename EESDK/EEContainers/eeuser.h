#ifndef EEUSER_H
#define EEUSER_H

#include <QString>

class EEUser
{
public:
    EEUser();

    QString email() const;
    void setEmail(const QString &email);

    QString created() const;
    void setCreated(const QString &created);

    bool isActivated() const;
    void setIsActivated(bool isActivated);

private:
    QString mEmail;
    QString mCreated;
    bool mIsActivated;
};

#endif // EEUSER_H
