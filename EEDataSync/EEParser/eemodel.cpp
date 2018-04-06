#include "eemodel.h"

EEModel::EEModel(QObject *parent) : QObject(parent)
{

}

QDateTime EEModel::updated() const
{
    return mUpdated;
}

void EEModel::setUpdated(const QDateTime &updated)
{
    mUpdated = updated;
}

QString EEModel::name() const
{
    return mName;
}

void EEModel::setName(const QString &name)
{
    mName = name;
}
