#include "eefolderparsecontroller.h"
#include "eefoldermodel.h"

#include <QString>
#include <QDirIterator>
#include <QFileSystemModel>
#include <QDebug>

#define ROOT    "/"

EEFolderParseController::EEFolderParseController(QObject *parent)
    : QObject(parent),
    mRootModel{new EEFolderModel}
{

}

EEFolderParseController::~EEFolderParseController()
{
    qDebug() << typeid(*this).name() << " : " << __FUNCTION__;

    delete mRootModel;
}

void EEFolderParseController::startSubfoldersInitialization(QString rootAddress)
{
    qDebug() << "- EEFolderParseController::startSubfoldersInitialization()";

    mRootAddress = rootAddress;
    //initialize root folder
    mRootModel->setName(rootAddress);

    qDebug() << "Folder to check: " << mRootModel->name();
    //initialize information about subfolders
    initializeListOfSubFoldersElements(mRootModel);
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

    model->cleanChildrenList();
    QDirIterator lIter(lPath, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    EEFolderModel *lChildModel = nullptr;
    EEModel *lFile = nullptr;
    QDateTime lUpdated;
    QDateTime lLastChildUpdate;

    while (lIter.hasNext() ) {
        QString lName = lIter.next();

        if (lIter.fileInfo().isDir()) {
            lChildModel = new EEFolderModel;

            lUpdated = lIter.fileInfo().lastModified();
            lChildModel->setName(lName.remove(0, mRootAddress.size()));
            //lChildModel->setUpdated(lUpdated);

            model->addFolder(lChildModel);

            lLastChildUpdate = initializeListOfSubFoldersElements(lChildModel);

            // if folder was modified later than setted value
            // or if its last update time haven't been initialized yet
            if (lLastChildUpdate > model->updated()
                    || model->updated().isNull()) {
                model->setUpdated(lLastChildUpdate);
            }
        } else if (lIter.fileInfo().isFile()) {
            if (lIter.fileInfo().size() > mMaximumFileSize) {
                qDebug() << "File is too big! Ignore it!";
            } else {
                lFile = new EEModel;

                lUpdated = lIter.fileInfo().lastModified();
                lFile->setName(lName.remove(0, mRootAddress.size()));
                lFile->setUpdated(lUpdated);

                model->addFile(lFile);

                if (lFile->updated() > model->updated()
                        || model->updated().isNull()) {
                    model->setUpdated(lFile->updated());

                }
            }
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


