#include "eemessagebox.h"
#include "globalconstants.h"

#include <QDebug>
#include <QCloseEvent>

EEMessageBox::EEMessageBox()
{
    this->setWindowTitle(GlobalData::gAppName);
}

void EEMessageBox::accept()
{
    setResult(QDialog::Accepted);
    hide();
}

void EEMessageBox::reject()
{
    setResult(QDialog::Rejected);
    hide();
}

void EEMessageBox::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void EEMessageBox::done(int r)
{
    if (r == 16384) {
        accept();
    } else {
        reject();
    }
}
