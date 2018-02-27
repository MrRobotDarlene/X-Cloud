#include "eemainwindow.h"
#include "ui_eemainwindow.h"

#include <QWebSocket>
#include "../XConfig/config.h"

EEMainWindow::EEMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EEMainWindow),
    mWebSocket(new QWebSocket())
{
    ui->setupUi(this);

    connect(mWebSocket, &QWebSocket::connected, this, &EEMainWindow::onConnected);
    typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
    connect(mWebSocket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors), this, &EEMainWindow::onSslErrors);

    QString lUrl(QStringLiteral("ws://localhost:"));
    lUrl.append(PORT);

    mWebSocket->open(QUrl(lUrl));
}

EEMainWindow::~EEMainWindow()
{
    delete mWebSocket;
    delete ui;
}

void EEMainWindow::onConnected()
{
    qDebug() << "WebSocket connected";
    connect(mWebSocket, &QWebSocket::textMessageReceived,  this, &EEMainWindow::onTextMessageReceived);
    mWebSocket->sendTextMessage(QStringLiteral("Hello, world!"));
}
//! [onConnected]

//! [onTextMessageReceived]
void EEMainWindow::onTextMessageReceived(QString message)
{
    qDebug() << "Message received:" << message;
    qApp->quit();
}

void EEMainWindow::onSslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    mWebSocket->ignoreSslErrors();
}

void EEMainWindow::error(QAbstractSocket::SocketError error)
{
    qDebug() << "Connection error" << error;
}
