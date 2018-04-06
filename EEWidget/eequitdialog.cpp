#include "eequitdialog.h"
#include "ui_eequitdialog.h"

#include <QCloseEvent>
#include <QApplication>
#include <QDebug>

EEQuitDialog::EEQuitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EEQuitDialog)
{
    ui->setupUi(this);
}

void EEQuitDialog::accept()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
    setResult(QDialog::Accepted);
    hide();
    QApplication::quit();
}

void EEQuitDialog::reject()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;
    setResult(QDialog::Rejected);
    hide();
}

EEQuitDialog::~EEQuitDialog()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    delete ui;
}
