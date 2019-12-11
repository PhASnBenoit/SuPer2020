#include "cserver.h"

CServer::CServer(QObject *parent, QString address, QString port) :
    QTcpServer(parent)
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << address << port << parent;
    setAddress(address);
    setPort(port);
    _clientConnectionErrorReceived = false;
    connect(this, SIGNAL(newConnection()), SLOT(onIncomingConnection()));
}

CServer::~CServer()
{
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    int i = 0;
    int count = _clientThreadMap.count();
    if (count <= 0) return;

    QMapIterator<CClientConnection*, CThread*> mapIterator(_clientThreadMap);

    while (mapIterator.hasNext())
    {
        i++;
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << i << "/"  << count;
        mapIterator.next();

        CClientConnection* clientConnection = mapIterator.key();
        CThread* thread = mapIterator.value();

        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete" << thread;
        delete thread;
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete" << clientConnection;
        delete clientConnection;
    }

    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "map.clear";
    _clientThreadMap.clear();
}

void CServer::setBdd(CBdd *bdd)
{
    _bdd = bdd;
}

CServer::SwitchOnState CServer::switchOn()
{
    SwitchOnState state = Success;

    if(! this->isListening())
    {
        bool ok = this->listen(QHostAddress::AnyIPv4, _port); // PhA 10/11/2019

        if(ok)
        {
            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "-> sig_switchedOn";

            setAddress(this->serverAddress().toString());
            setPort(QString::number(this->serverPort()));
            _clientConnectionErrorReceived = false;

            emit sig_switchedOn();
        }
        else
        {
            QAbstractSocket::SocketError socketError = this->serverError();

            switch(socketError)
            {
            case QAbstractSocket::SocketAddressNotAvailableError:
                state = this->AddressNotAvailableError;
                break;

            case QAbstractSocket::SocketAccessError:
                state = this->PortProtectedError;
                break;

            case QAbstractSocket::AddressInUseError:
                state = this->PortAlreadyInUseError;
                break;

            default:
                state = this->UnknownError;
            }

            qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "erreur listen, return " << state;
        }
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "ignore (deja en ecoute), return" << state;

    return state;
}

void CServer::switchOff()
{
    if(this->isListening())
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "close -> sig_switchedOff";
        this->close();
        emit sig_switchedOff();
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
}


void CServer::closeAllClientConnection()
{
    emit sig_closeAllClientConnection();
}


void CServer::onIncomingConnection()
{
    qintptr sd = nextPendingConnection()->socketDescriptor();
    qDebug() << "Lecteur entrant " << QThread::currentThreadId() << Q_FUNC_INFO << sd;

    CClientConnection* clientConnection;
    clientConnection = new CClientConnection(sd, _bdd);
    CThread* thread;
    thread = new CThread;
    _clientThreadMap.insert(clientConnection, thread); // mémorise le thread et client TCP
    clientConnection->moveToThread(thread);

    // Connexions
    // Thread::started() déclenche clientConnection::open()
    clientConnection->connect(thread, SIGNAL(started()), SLOT(open()));
    // Une erreur chez un client entrainera l'arrêt de l'écoute du serveur
    connect(clientConnection, SIGNAL(sig_error(QString)), SLOT(clientConnection_error(QString)));
    // Le serveur surveille les déconnexions de ses clients pour libérer les ressources
    connect(clientConnection, SIGNAL(sig_disconnected()), SLOT(clientConnection_disconnected()));
    // Le signal closeAllClientConnection déclenchera le close() de TOUS les clientconnection
    clientConnection->connect(this, SIGNAL(sig_closeAllClientConnection()), SLOT(close()));
    // clientConnection::sig_disconnected() déclenchera sa propre destruction
    //clientConnection->connect(clientConnection, SIGNAL(sig_disconnected()), SLOT(deleteLater()));
    // L'arrêt du thread entrainera la destruction de clientConnection
    //clientConnection->connect(thread, SIGNAL(finished()), SLOT(deleteLater()));
    // La destruction de clientConnection déclenchera l'arrêt du thread
    //thread->connect(clientConnection, SIGNAL(destroyed()), SLOT(quit()));
    // L'arrêt du thread déclenche la destruction de clientconnection
    //clientConnection->connect(thread, )
    // L'arrêt du thread déclenchera sa propre destruction
    //thread->connect(thread, SIGNAL(finished()), SLOT(deleteLater()));
    // TODO : Traiter de la même façon le signal terminated ?
    // La destruction de clientConnection déclenchera la destruction du thread
    //thread->connect(clientConnection, SIGNAL(destroyed()), SLOT(deleteLater()));

    emit sig_newConnection(*clientConnection);

    // Lancement du thread
    thread->start();
}

bool CServer::setAddress(QString address)
{
    bool ok;
    QHostAddress addressQHost;

    ok = addressQHost.setAddress(address);

    if(ok && (addressQHost!=_address))
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << address << "-> sig_addressChanged, return" << ok;

        _address.setAddress(address);
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << address << "ignore (mauvais format ou meme valeur), return" << ok;

    return ok;
}

bool CServer::setPort(QString port)
{
    bool ok;
    quint16 portQuint16 = port.toUInt(&ok); // quint16 <=> ulong

    if(ok && portQuint16 != _port)
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << port << "-> sig_portChanged, return" << ok;

        _port = portQuint16;
        emit sig_portChanged(portQuint16);
    }
    else
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << port << "ignore (mauvais format ou meme valeur), return" << ok;

    return ok;
}

void CServer::clientConnection_error(QString error)
{
    // Seulement si aucune erreur n'a été rencontrée depuis le dernier SwitchOn
    if(!_clientConnectionErrorReceived)
    {
        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;

        // Mémoriser le passage ici pour ne pas avoir à le faire plusieurs fois
        _clientConnectionErrorReceived = true;

        // Stopper l'écoute pour eviter un flood de demande de connexion en cas d'erreur répétitive
        switchOff();

        // Et le signaler
        emit sig_switchedOffOnError(error);
    }
}

void CServer::clientConnection_disconnected()
{
    CClientConnection *sender = static_cast<CClientConnection *>(QObject::sender());
    CThread *thread = _clientThreadMap.take(sender);
    QString adrLect = sender->getClientAddress();
    _clientThreadMap.remove(sender);  // suppression de la liste
    emit sigDisconnected("Fin du client lecteur "+adrLect);
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete objet ClientConnexion lecteur" << sender;
    delete sender;
    emit sigDisconnected("Fin du thread lecteur "+adrLect);
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "delete thread lecteur" << thread;
    delete thread;
}

QDebug operator<<(QDebug debug, const CServer::SwitchOnState& state)
{
    QString stateString;

    switch(state)
    {
    case CServer::Success:
        stateString = "Success";
        break;
    case CServer::AddressNotAvailableError:
        stateString = "AddressNotAvailableError";
        break;
    case CServer::PortProtectedError:
        stateString = "PortProtectedError";
        break;
    case CServer::PortAlreadyInUseError:
        stateString = "PortAlreadyInUseError";
        break;
    case CServer::UnknownError:
        stateString = "UnknownError";
        break;
    } // sw
    debug << stateString;
    return debug.nospace();
}
