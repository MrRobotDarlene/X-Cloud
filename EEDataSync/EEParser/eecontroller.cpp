#include "eecontroller.h"
#include "eefoldermodel.h"

#include <QString>
#include <QDirIterator>
#include <QFileSystemModel>
#include <QDebug>

#define ROOT    "Root"

EEController::EEController(QString rootAddress, QObject *parent)
    : QObject(parent),
    mRootAddress{rootAddress},
    mRootModel{new EEFolderModel}
{
    //initialize root folder
    mRootModel->setName(rootAddress);

    //initialize information about subfolders
    initializeListOfSubFoldersElements(mRootModel);
}

EEController::~EEController()
{
    delete mRootModel;
}
/**
 * @brief EEController::initializeListOfSubFoldersElements
 * Initialize folders names, last updated and subfolders list
 * starting from root folder
 * @param model - root folder
 * @return last update of folder
 */
QDateTime EEController::initializeListOfSubFoldersElements(EEFolderModel *model)
{
    QDirIterator lIter(model->name(), QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    if (mRootAddress == model->name()) {
        model->setName(ROOT);
    }
    EEFolderModel *lChildModel;
    EEModel *lFile;
    QDateTime lUpdated;
    QDateTime lLastChildUpdate;

    while (lIter.hasNext() ) {
        QString lName = lIter.next();
        lName = lName.remove(0, mRootAddress.size()+1);

        if (lIter.fileInfo().isDir()) {
            lChildModel = new EEFolderModel;

            lUpdated = lIter.fileInfo().lastModified();
            lChildModel->setName(lName);
            lChildModel->setUpdated(lUpdated);

            model->addFolder(lChildModel);

            lLastChildUpdate = initializeListOfSubFoldersElements(lChildModel);

            // if folder was modified later than setted value
            // or if its last update time haven't been initialized yet
            if (lLastChildUpdate.secsTo(model->updated()) < 0
                    || model->updated().isNull()) {
                model->setUpdated(lLastChildUpdate);
            }
        } else if (lIter.fileInfo().isFile()) {
            lFile = new EEModel;

            lUpdated = lIter.fileInfo().lastModified();
            lFile->setName(lName);
            lFile->setUpdated(lUpdated);

            model->addFile(lFile);
        }
    }

    return model->updated();
}

EEFolderModel *EEController::rootModel() const
{
    return mRootModel;
}
