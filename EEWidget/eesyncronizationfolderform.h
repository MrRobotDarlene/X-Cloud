#ifndef EESYNCRONIZATIONFOLDERFORM_H
#define EESYNCRONIZATIONFOLDERFORM_H

#include <EEWidget/eewidget.h>
class EESDK;

namespace Ui {
class EESyncronizationFolderForm;
}
/**
 * @brief The EESyncronizationFolderForm class
 * Form, which provides user possibility to choose folder,
 * which will be syncronizated with cloud
 */
class EESyncronizationFolderForm :  public EEWidget
{
    Q_OBJECT

public:
    explicit EESyncronizationFolderForm(EESDK *sdk, QWidget *parent = 0);
    ~EESyncronizationFolderForm();
    void resetDefaultPath();
signals:
    void newFolderChoosed(QString);
private slots:
    void on_buttonChooseFolder_clicked();

    void on_buttonAccept_clicked();

    void on_buttonCancel_clicked();

private:
    Ui::EESyncronizationFolderForm *ui;
};

#endif // EESYNCRONIZATIONFOLDERFORM_H
