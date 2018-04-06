#ifndef EEMESSAGEBOX_H
#define EEMESSAGEBOX_H

#include <QMessageBox>

class QCloseEvent;

class EEMessageBox : public QMessageBox
{
public:
    EEMessageBox();
private slots:
    void accept();
    void reject();
    void closeEvent(QCloseEvent *event);
    void done(int r);
};

#endif // EEMESSAGEBOX_H

