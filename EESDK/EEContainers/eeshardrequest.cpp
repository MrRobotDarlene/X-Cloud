#include "eeshardrequest.h"

EEShardRequest::EEShardRequest()
{

}

QStringList EEShardRequest::challenges() const
{
    return mChallenges;
}

void EEShardRequest::appendChallenge(const QString &ch)
{
    mChallenges.append(ch);
}

QStringList EEShardRequest::tree() const
{
    return mTree;
}

void EEShardRequest::appendTree(const QString &tree)
{
    mTree.append(tree);
}

QStringList EEShardRequest::exclude() const
{
    return mExclude;
}

void EEShardRequest::appendExclude(const QString &exclude)
{
    mExclude.append(exclude);
}

void EEShardRequest::setTree(const QStringList &tree)
{
    mTree = tree;
}

void EEShardRequest::setChallenges(const QStringList &challenges)
{
    mChallenges = challenges;
}
