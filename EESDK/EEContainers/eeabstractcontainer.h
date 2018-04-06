#ifndef EEABSTRACTCONTAINER_H
#define EEABSTRACTCONTAINER_H

#include <QString>
#include <QDateTime>

class EEAbstractContainer
{
public:
    explicit EEAbstractContainer();
    EEAbstractContainer(EEAbstractContainer *b);

    QString user() const;
    void setUser(const QString &user);

    bool isActive() const;
    void setIsActive(bool isActive);

    QDateTime created() const;
    void setCreated(const QDateTime &created);

    QString id() const;
    void setId(const QString &id);

    int storageSize() const;
    void setStorageSize(int storageSize);

private:
    QString mUser;
    int mIsActive;
    int mStorageSize;
    QDateTime mCreated;
    QString mId;
};

#endif // EEABSTRACTCONTAINER_H
