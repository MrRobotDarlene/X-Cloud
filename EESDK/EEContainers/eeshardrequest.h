#ifndef EESHARDREQUEST_H
#define EESHARDREQUEST_H

#include "eeshardbasic.h"

#include <QStringList>

class EEShardRequest : public EEShardBasic
{
public:
    EEShardRequest();

    QStringList challenges() const;
    void appendChallenge(const QString&);

    QStringList tree() const;
    void appendTree(const QString &);

    QStringList exclude() const;
    void appendExclude(const QString &exclude);

    void setTree(const QStringList &tree);
    void setChallenges(const QStringList &challenges);

private:
    QStringList mChallenges;
    QStringList mTree;
    QStringList mExclude;
};

#endif // EESHARDREQUEST_H
