#include "eequitdialog.h"
#include "ui_eequitdialog.h"
#include "globalconstants.h"

#include <QCloseEvent>
#include <QApplication>
#include <QDebug>

EEQuitDialog::EEQuitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EEQuitDialog)
{
    ui->setupUi(this);

    this->setWindowTitle(GlobalData::gAppName);
}

void EEQuitDialog::accept()
{
    setResult(QDialog::Accepted);
    hide();
    QApplication::quit();
}

void EEQuitDialog::reject()
{    setResult(QDialog::Rejected);
    hide();
}

EEQuitDialog::~EEQuitDialog()
{
    delete ui;
}
