#ifndef EESTATUSTIMER_H
#define EESTATUSTIMER_H

#include <QObject>
class QTimer;
/**
 * @brief The EEStatusTimer class
 * CLass(singelton), which provides check of folders status by timer.
 * Check happening once per minute.
 */
class EEStatusTimer : public QObject
{
    Q_OBJECT
public:
    static EEStatusTimer &getInstance();
    void run();
    bool isRunning();

    EEStatusTimer(const EEStatusTimer&) = delete;
    EEStatusTimer& operator=(const EEStatusTimer&) = delete;

    EEStatusTimer(const EEStatusTimer&&) = delete;
    EEStatusTimer& operator=(const EEStatusTimer&&) = delete;
private:
    explicit EEStatusTimer(QObject *parent = 0);
    ~EEStatusTimer();

signals:
    void checkStatusByTimer();

public slots:
    void startTimer();
    void stopCheck();
    void checkStatus();
private:
    QTimer *mTimer;
    static constexpr int mInterval = 60000;//86400000;
};

#endif // EESTATUSTIMER_H
