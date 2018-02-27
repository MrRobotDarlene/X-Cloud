#ifndef EESHARDBASIC_H
#define EESHARDBASIC_H

#include <QString>

class EEShardBasic
{
public:
    EEShardBasic();

    QString hash() const;
    void setHash(const QString &hash);

    int size() const;
    void setSize(int size);

    int index() const;
    void setIndex(int index);

private:
    int mIndex;
    QString mHash;
    int mSize;
};

#endif // EESHARDBASIC_H
