#include "ctcpsocket.h"

CTcpSocket::CTcpSocket(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_error(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(hostFound()), this, SLOT(slot_hostFound()));
    connect(this, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(slot_proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slot_stateChanged(QAbstractSocket::SocketState)));
    connect(this, SIGNAL(aboutToClose()), this, SLOT(slot_aboutToClose()));
    connect(this, SIGNAL(readyRead()), this, SLOT(slot_readyRead()));
    connect(this, SIGNAL(readChannelFinished()), this, SLOT(slot_readChannelFinished()));
    connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(slot_bytesWritten(qint64)));
}

CTcpSocket::~CTcpSocket()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_connected()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_disconnected()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_error(QAbstractSocket::SocketError error)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << error;
}

void CTcpSocket::slot_hostFound()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_stateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << state;
}

void CTcpSocket::slot_aboutToClose()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_readyRead()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_readChannelFinished()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}

void CTcpSocket::slot_bytesWritten(qint64 number)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << number;
}
