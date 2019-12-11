#ifndef CSERVER_H
#define CSERVER_H

#include <QMap>
#include <QThread>
#include <QDebug>
#include <QTcpServer>
#include "cclientconnection.h"
#include "cthread.h"
#include "cbdd.h"

class CServer : public QTcpServer
{
    Q_OBJECT

public:
    enum SwitchOnState
    {
        Success,
        AddressNotAvailableError,
        PortProtectedError,
        PortAlreadyInUseError,
        UnknownError
    };

signals:
    void sig_switchedOn();
    void sig_switchedOff();
    void sig_switchedOffOnError(QString);
    void sigDisconnected(QString str);
    void sig_portChanged(quint16);
    void sig_addressChanged(QString);
    void sig_newConnection(const CClientConnection &lect);
    void sig_closeAllClientConnection();

public slots:
    SwitchOnState switchOn();
    void switchOff();
    void closeAllClientConnection();

public:
    explicit CServer(QObject *parent = nullptr, QString address = "192.168.7.200", QString port = "2020");
    ~CServer();
    void setBdd(CBdd *bdd);

private slots:
    void onIncomingConnection();
    void clientConnection_error(QString);
    void clientConnection_disconnected();

private:
    bool setAddress(QString);
    bool setPort(QString);

    QHostAddress _address;
    quint16 _port;
    CBdd *_bdd;  // sauve le lien vers la BDD (PhA le 2018-04-10)

    QMap<CClientConnection*, CThread*> _clientThreadMap;
    bool _clientConnectionErrorReceived;
};

QDebug operator<<(QDebug, const CServer::SwitchOnState&);

#endif // CSERVER_H
