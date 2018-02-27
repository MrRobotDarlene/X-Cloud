#include "eefolderparsecontroller.h"
#include "eefoldermodel.h"

#include <QString>
#include <QDirIterator>
#include <QFileSystemModel>
#include <QDebug>

#define ROOT    "/"

EEFolderParseController::EEFolderParseController(QString rootAddress, QObject *parent)
    : QObject(parent),
    mRootAddress{rootAddress},
    mRootModel{new EEFolderModel}
{
    //initialize root folder
    mRootModel->setName(rootAddress);

    //initialize information about subfolders
    initializeListOfSubFoldersElements(mRootModel);
}

EEFolderParseController::~EEFolderParseController()
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
QDateTime EEFolderParseController::initializeListOfSubFoldersElements(EEFolderModel *model)
{
    QString lPath = filePath(model->name());
    if (mRootAddress == model->name()) {
        lPath = mRootAddress;
        model->setName(ROOT);
    }

    QDirIterator lIter(lPath, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    EEFolderModel *lChildModel = nullptr;
    EEModel *lFile = nullptr;
    QDateTime lUpdated;
    QDateTime lLastChildUpdate;

    while (lIter.hasNext() ) {
        QString lName = lIter.next();
        //lName = lName.remove(0, mRootAddress.size()+1);

        if (lIter.fileInfo().isDir()) {
            lChildModel = new EEFolderModel;

            lUpdated = lIter.fileInfo().lastModified();
            lChildModel->setName(lName.remove(0, mRootAddress.size()));
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
            lFile->setName(lName.remove(0, mRootAddress.size()));
            lFile->setUpdated(lUpdated);

            model->addFile(lFile);
        }
    }

    return model->updated();
}

EEFolderModel * const EEFolderParseController::rootModel() const
{
    return mRootModel;
}

QString EEFolderParseController::filePath(QString fileName)
{
    return mRootAddress + fileName;
}
