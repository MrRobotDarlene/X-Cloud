#include "eestatustimer.h"
#include <QDebug>
#include <QTimer>

#include <typeinfo>

EEStatusTimer &EEStatusTimer::getInstance()
{
    static EEStatusTimer instanse;
    return instanse;
}

void EEStatusTimer::run()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
    this->checkStatus();
}

bool EEStatusTimer::isRunning()
{
    return mTimer->isActive();
}

EEStatusTimer::EEStatusTimer(QObject *parent) : QObject(parent)
{
    mTimer = new QTimer();
    connect(mTimer, SIGNAL(timeout()), this, SLOT(checkStatus()));
}

EEStatusTimer::~EEStatusTimer()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    mTimer->stop();
    delete mTimer;
}

void EEStatusTimer::startTimer()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
    mTimer->start(mInterval);
}

void EEStatusTimer::stopCheck()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
    if (mTimer->isActive()) {
        mTimer->stop();
    }
}

void EEStatusTimer::checkStatus()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
    qDebug() << "CHECK STATUS BY TIMER";

    emit checkStatusByTimer();
}
