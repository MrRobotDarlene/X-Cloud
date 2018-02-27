#ifndef EEFARMER_H
#define EEFARMER_H

#include <QString>
#include <QHostAddress>

class EEFarmer
{
public:
    EEFarmer();

    QString userAgent() const;
    void setUserAgent(const QString &userAgent);

    QString protocol() const;
    void setProtocol(const QString &protocol);

    QHostAddress address() const;
    void setAddress(const QHostAddress &address);
    void setAddress(const QString &address);

    QString port() const;
    void setPort(const QString &port);

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QString lastSeen() const;
    void setLastSeen(const QString &lastSeen);

private:
    QString mUserAgent;
    QString mProtocol;
    QHostAddress mAddress;
    QString mPort;
    QString mNodeId;
    QString mLastSeen;
};

#endif // EEFARMER_H
