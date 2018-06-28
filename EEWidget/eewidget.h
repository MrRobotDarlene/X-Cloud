
#ifndef EEWIDGET_H
#define EEWIDGET_H

#include <QWidget>

#include "../EESDK/eesdk.h"

class EEWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EEWidget(EESDK *sdk, QWidget *parent = 0);
    virtual ~EEWidget();
    EESDK *sdk() const;
    void setSdk(EESDK *sdk);
    void show();
protected:
    void closeEvent(QCloseEvent *event);
    void moveEvent(QMoveEvent *event);
private:
    EESDK *mSdk;
};

#endif // EEWIDGET_H
