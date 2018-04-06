#include "eelog.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>


#ifdef QT_DEBUG
#include <iostream>
#endif


/**
 * @brief EELog::getInstance
 * @return
 */
EELog *EELog::getInstance() {
    static EELog instance;
    return &instance;
}

/**
 * @brief EELog::EELog Initialise defaul Log instance.
 * @param parent
 */
EELog::EELog(QObject *parent) :
    QObject(parent),
    mLogFileName{"app_log.txt"},
    mLogFilePath{ QCoreApplication::applicationDirPath() + QString("//temp//")},
    mLogLinesCounter{0}
{
    QString lPathToTemp = QCoreApplication::applicationDirPath() + QString("//temp//");
    if (!QDir(lPathToTemp).exists()) {
        qDebug() << "Create log folder";
        QDir().mkdir(lPathToTemp);
    } else {
        qDebug() << "Log folder already exists";
    }
}

/**
 * @brief EELog::initialise
 */
void EELog::initialise() {

    // Set default file name for output log.
    mOutLogFile = new QFile(getLogFileAbsolutePath());

    // Set default message handler for debug log.
    qInstallMessageHandler(DefaultMessageHandler);

    // Open current log file and read lines count.
    if (mOutLogFile->open(QIODevice::ReadOnly)) {
        QTextStream inputStream(mOutLogFile);
        while (!inputStream.atEnd()) {
            inputStream.readLine();
            mLogLinesCounter ++;
        }
        mOutLogFile->close();
    }
}


/**
 * @brief EELog::writeLog
 * @param text
 */
void EELog::writeLog(const QString &text) {

    // Check if the log file is overloaded and clear if true,
    // otherwise continue append lines to file.
    if (mLogLinesCounter >= mDefaultLogMaxLine) {
        mLogLinesCounter = 0;
        mOutLogFile->open(QIODevice::ReadWrite | QIODevice::Truncate);
    } else {
        mOutLogFile->open(QIODevice::WriteOnly | QIODevice::Append);
    }

    QTextStream outTextStream(mOutLogFile);
    outTextStream << text << endl;
    mOutLogFile->flush();
    mOutLogFile->close();

    // Increment lines count.
    mLogLinesCounter ++;
}

QString EELog::getLogFileAbsolutePath() const {
    return mLogFilePath + "/" + mLogFileName;
}

QString EELog::getLogFilePath() const {
    return mLogFilePath;
}

void EELog::setLogFilePath(const QString &value){
    mLogFilePath = value;
    mLogFileAbsolutePath = mLogFilePath + mLogFileName;
}

QString EELog::getLogFileName() const{
    return mLogFileName;
}

void EELog::setLogFileName(const QString &value){
    mLogFileName = value;
    mLogFileAbsolutePath = mLogFilePath + mLogFileName;
}

/**
 * @brief EELog::SetDefaultMessageOutputLog
 * @param type
 * @param context
 * @param msg
 */
void EELog::DefaultMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    (void)context; //ignoring unused parameter
    QString lMessage;
    QString timeStamp = QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd hh:mm::ss");
    switch (type) {
        case QtDebugMsg: {
            lMessage = QString(timeStamp + " Debug: %1").arg(QString(msg));
            break;
        }
        case QtWarningMsg: {
            lMessage = QString(timeStamp + " Warning: %1").arg(QString(msg));
            break;
        }
        case QtCriticalMsg: {
            lMessage = QString(timeStamp + " Critical: %1").arg(QString(msg));
            break;
        }
        case QtFatalMsg: {
            lMessage = QString(timeStamp + " Fatal: %1").arg(QString(msg));
            break;
        }
    }

#ifdef QT_DEBUG
    std::cout << lMessage.toStdString() << std::endl;
#endif

    EELog::getInstance()->writeLog(lMessage);
}
