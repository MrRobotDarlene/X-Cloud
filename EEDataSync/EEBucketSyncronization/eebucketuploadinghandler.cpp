#include "eebucketuploadinghandler.h"
#include "EEDataSync/EELocalDataParser/eefoldermodel.h"
#include "eebucketdataholder.h"

#include <QQueue>

EEBucketUploadingHandler::EEBucketUploadingHandler(EEBucketDataHolder *bucketData):
    EEBucketBaseHandler{bucketData},
    EEFolderModelHolder{}
{
}

EEBucketUploadingHandler::~EEBucketUploadingHandler()
{

}

QString EEBucketUploadingHandler::currentFileNameFromFolderModel() const
{
    return currentFolderModel()->filesList().at(bucketData()->currentFileIndex())->name();
}

void EEBucketUploadingHandler::addModelToFilesQueue(QString fileName, QString bucketId)
{
    EEBucketBaseHandler::addModelToFilesQueue(fileName, bucketId);
}

