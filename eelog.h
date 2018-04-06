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

private:
    static void DefaultMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

public:
    void initialise();

    QString getLogFileName() const;
    void setLogFileName(const QString &value);

    QString getLogFilePath() const;
    void setLogFilePath(const QString &value);

    QString getLogFileAbsolutePath() const;

private :
    EELog(QObject *parent = 0);

    void writeLog(const QString& text);

private:
    QFile* mOutLogFile;

    QString mLogFileName;
    QString mLogFilePath;
    QString mLogFileAbsolutePath;

    int mLogLinesCounter;

    static constexpr int mDefaultLogMaxLine = 1000;
signals:

public slots:
};

#endif // EELOG_H


