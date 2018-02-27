#include "eefarmer.h"

EEFarmer::EEFarmer()
{

}

QString EEFarmer::userAgent() const
{
    return mUserAgent;
}

void EEFarmer::setUserAgent(const QString &userAgent)
{
    mUserAgent = userAgent;
}

QString EEFarmer::protocol() const
{
    return mProtocol;
}

void EEFarmer::setProtocol(const QString &protocol)
{
    mProtocol = protocol;
}

QHostAddress EEFarmer::address() const
{
    return mAddress;
}

void EEFarmer::setAddress(const QHostAddress &address)
{
    mAddress = address;
}

void EEFarmer::setAddress(const QString &address)
{
    mAddress = QHostAddress(address);
}

QString EEFarmer::port() const
{
    return mPort;
}

void EEFarmer::setPort(const QString &port)
{
    mPort = port;
}

QString EEFarmer::nodeId() const
{
    return mNodeId;
}

void EEFarmer::setNodeId(const QString &nodeId)
{
    mNodeId = nodeId;
}

QString EEFarmer::lastSeen() const
{
    return mLastSeen;
}

void EEFarmer::setLastSeen(const QString &lastSeen)
{
    mLastSeen = lastSeen;
}
