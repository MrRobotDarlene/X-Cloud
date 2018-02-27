#ifndef EESHARD_H
#define EESHARD_H

#include "eefarmer.h"
#include "eeshardbasic.h"

#include <QString>

class EEShard : public EEShardBasic
{
public:
    EEShard();

    bool isPartial() const;
    void setIsPartial(bool isPartial);

    QString token() const;
    void setToken(const QString &token);

    QString operation() const;
    void setOperation(const QString &operation);

    EEFarmer farmer() const;
    void setFarmer(const EEFarmer &farmer);

private:
    bool mIsPartial;
    QString mToken;
    QString mOperation;
    EEFarmer mFarmer;
};

#endif // EESHARD_H
