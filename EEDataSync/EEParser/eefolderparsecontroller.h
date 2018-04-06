#ifndef EECONTROLLER_H
#define EECONTROLLER_H

#include <QObject>
#include <QDateTime>

class EEFolderModel;

/**
 * @brief The EEController class
 * Controller, which handle work of the folder information(EEFolderModel)
 * and json builder(EEJsonBuilder) used to create json document based on this information folder
 */
class EEFolderParseController: public QObject
{
    Q_OBJECT
public:
    explicit EEFolderParseController(QObject *parent = nullptr);

    ~EEFolderParseController();

    EEFolderModel *const rootModel() const;

    QString filePath(QString fileName);

    void startSubfoldersInitialization(QString rootAddress);
private:
    /**
     * @brief EEController::initializeListOfSubFoldersElements
     * Initialize folders names, last updated and subfolders list
     * starting from root folder
     * @param model - root folder
     * @return last update of folder
     */
    QDateTime initializeListOfSubFoldersElements(EEFolderModel *model);
private:
    QString mRootAddress;
    EEFolderModel *mRootModel;
    static constexpr int mMaximumFileSize = 3000000;
};

#endif // EECONTROLLER_H
