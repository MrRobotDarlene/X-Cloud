#ifndef EEMODEL_H
#define EEMODEL_H

#include <QObject>
#include <QDateTime>

class EEModel : public QObject
{
    Q_OBJECT
public:
    explicit EEModel(QObject *parent = nullptr);

    /**
     * @brief updated
     * @return
     */
    QDateTime updated() const;
    /**
     * @brief setUpdated
     * @param updated
     */
    void setUpdated(const QDateTime &updated);

    /**
     * @brief name
     * @param name
     */
    QString name() const;
    /**
     * @brief setName
     * @return
     */
    void setName(const QString &name);

    inline bool operator ==(EEModel& model) {
        return (this->name() == model.name() && this->updated() == model.updated());
    }

private:
    QDateTime mUpdated;
    QString mName;
};

#endif // EEMODEL_H
