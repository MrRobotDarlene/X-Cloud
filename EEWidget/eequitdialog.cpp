#include "eequitdialog.h"
#include "ui_eequitdialog.h"

#include <QCloseEvent>

EEQuitDialog::EEQuitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EEQuitDialog)
{
    ui->setupUi(this);
}

void EEQuitDialog::accept()
{
    setResult(QDialog::Accepted);
    hide();
}

void EEQuitDialog::reject()
{
    setResult(QDialog::Rejected);
    hide();
}

EEQuitDialog::~EEQuitDialog()
{
    delete ui;
}
