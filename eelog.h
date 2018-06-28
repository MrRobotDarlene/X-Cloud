#ifndef EELOG_H
#define EELOG_H

#include <QObject>
#include <QtGlobal>
class QFile;

/**
 * @brief The EELog class
 * Class, which provides logs
 */
class EELog : public QObject
{
    Q_OBJECT

public:
    static EELog* getInstance();

    void initialise();

    QString getLogFileName() const;
    void setLogFileName(const QString &value);

    QString getLogFilePath() const;
    void setLogFilePath(const QString &value);

    QString getLogFileAbsolutePath() const;

    static const QString mlogFileName;

private :
    EELog(QObject *parent = 0);

    void writeLog(const QString& text);

    static void DefaultMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);


private:
    QFile* mOutLogFile;

    QString mLogFileName;
    QString mLogFilePath;
    QString mLogFileAbsolutePath;

    int mLogLinesCounter;

    static constexpr int mDefaultLogMaxLine = 500;
};

#endif // EELOG_H


