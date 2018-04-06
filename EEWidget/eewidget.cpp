#include "eewidget.h"

#include <QCloseEvent>
#include <QMoveEvent>

EEWidget::EEWidget(EESDK *sdk, QWidget *parent) :
    QWidget(parent)
{
    mSdk = sdk;
}
void EEWidget::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}
void EEWidget::moveEvent(QMoveEvent *event)
{
    this->resize(width(),height());
    QWidget::moveEvent(event);
}
EESDK *EEWidget::sdk() const
{
    return mSdk;
}
void EEWidget::setSdk(EESDK *sdk)
{
    mSdk = sdk;
}

void EEWidget::show()
{
    QWidget::show();

    raise();
}
