#ifndef EEQUITDIALOG_H
#define EEQUITDIALOG_H

#include <QDialog>

namespace Ui {
class EEQuitDialog;
}

class EEQuitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EEQuitDialog(QWidget *parent = 0);
    ~EEQuitDialog();

private slots:
    void accept();
    void reject();

private:
    Ui::EEQuitDialog *ui;
};

#endif // EEQUITDIALOG_H
