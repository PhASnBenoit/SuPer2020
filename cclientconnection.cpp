#include "cclientconnection.h"


QMutex CClientConnection::_mutex;

CClientConnection::CClientConnection(qintptr sd, CBdd *bdd) :
    QObject(), _socketDescriptor(sd), _opened(false), _isAReader(false), _clientAddress(""), _bufferDataRead("")
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << sd;

    _bdd = bdd;
    _SyncDeb = _SyncFin = false;
    _tcpSocket.setParent(this);
    this->connect(&_tcpSocket, SIGNAL(disconnected()), SIGNAL(sig_disconnected()));
    // TODO this->connect(&_tcpSocket, SIGNAL(disconnected()), SLOT(deleteLater())); // C'est Server qui crée des ClientConnection, c'est donc Server qui décide quand les détruire
    this->connect(&_tcpSocket, SIGNAL(disconnected()), SLOT(on_disconnected()));
}

CClientConnection::~CClientConnection()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    _tcpSocket.close();
    on_disconnected();
}

void CClientConnection::open()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;

    if(! _opened)
    {
        _tcpSocket.setSocketDescriptor(_socketDescriptor);
        if(_tcpSocket.isValid())
        {
            _opened = true;
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "_tcpSocket.isValid()," << _opened;
             // Activation de l'option KeepAlive
            _tcpSocket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
            // Récupère l'adresse du client.
            _clientAddress = _tcpSocket.peerAddress().toString();
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "clientAddress:" << _clientAddress;
            filter();
        }
        else
        {
            emit sig_error("socket invalid"); // TODO : Utiliser un enum ?
        }
    }
}

void CClientConnection::close()
{
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    _tcpSocket.close();
    emit sig_closed(); // TODO : Supprimer ?
}

QString CClientConnection::getClientAddress()
{
    return _clientAddress;
}

//////////////////////////////////////
void CClientConnection::filter()
{
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;

    // Demande à la BDD si c'est un lecteur.
    CReader reader;
    int numLect, typLect, numLieu;
    QString radr;

    int nb = _bdd->getLecteur(_clientAddress, &numLect, &numLieu, &typLect, &radr);
    if(nb == 0) {  // pas un lecteur

        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "query.size() == 0 -> isNotAReader";
        // Signale la détection d'un intrus
        emit sig_isNotAReader(_clientAddress);
        // Le laisser connecté (évite le flood de connexion déconnexion)
        // Mais lui vider son buffer dès qu'il se remplit
        this->connect(&_tcpSocket, SIGNAL(readyRead()), SLOT(bleedBuffer()));

    } else {

        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [lecteur existe dans BDD ?] ok";
        reader.number(numLect);
        reader.placeId(numLieu);
        reader.address(radr);
        reader.setConnected(true);
        _isAReader = true;
        // Update BDD (lecteur connecté)
        _bdd->setEtatLect(numLect, reader.isConnected());

        qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [Update lecteur.estConnecte] ok -> sig_isAReader(reader)";
        qRegisterMetaType<CReader>("Reader");

        // Signale la détection d'un lecteur
        emit sig_isAReader(reader);

        // Traiter les trames reçues
        _tcpSocket.readAll(); // purge avant le démarrage
        this->connect(&_tcpSocket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    } // else
}

//////////////////////////////
/// \brief ClientConnection::onReadyRead
#define CARDEB '['
#define CARFIN ']'
void CClientConnection::onReadyRead()
{
    QString dataRead;
    int fin;

    QString regexFrameString("\\[[0-9A-Fa-f]{10}\\]");
    QRegExp validFrameRegex(regexFrameString);

    dataRead = _tcpSocket.readAll();
    _bufferDataRead += dataRead;

    qDebug() << "Reçu : " << dataRead << " et en mémoire avant traitement : " << _bufferDataRead;

    while (_bufferDataRead.size() >= 12)  {
        if (!_SyncDeb) {
            for (int i=0 ; i<_bufferDataRead.size() ; i++) {
                if (_bufferDataRead[i]==CARDEB) {
                    _SyncDeb = true;
                    _SyncFin = false;
                    _bufferDataRead = _bufferDataRead.mid(i);  // on enlève les éventuels cars mauvais précédents
                    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Début de trame détecté ! ";
                    break;  // arrêt de la recherche lorsque premier [ trouvé
                } // if CARDEB
            } // for
        } // if !mSynchDeb

        if (!_SyncFin && _SyncDeb) {
            for (int i=0 ; i<_bufferDataRead.size() ; i++) {
                if (_bufferDataRead[i]==CARFIN) {
                    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Fin de trame détecté : " << _bufferDataRead.left(i+1);
                    _SyncFin = true;
                    fin = i;
                    break;
                } // if CARFIN
            } // for
        }  // if !mSyncFin

        if (_SyncDeb && _SyncFin) {
            // ICI j'ai au moins une trame dans le buffer
            QString validFrameString = _bufferDataRead.left(fin+1);   // validFrameRegex.cap(0);
            if(validFrameString.contains(validFrameRegex)) {  // si trame bon format
                qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "validFrameString :" << validFrameString;
            } else {
                qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "!!! Format non valide :" << validFrameString;
                validFrameString = "?"+validFrameString;  // ? signale l'erreur dans l'IHM
            } //else
            _bufferDataRead = _bufferDataRead.mid(fin+1); // on enleve la partie avant le ]
            emit sig_frameReceived(validFrameString);
        } // if syn deb et fin
        _SyncDeb = _SyncFin = false;
    } // wh
    qDebug() << "En mémoire : " << _bufferDataRead;
} // méthode



/*
//////////////////////////////
/// \brief ClientConnection::onReadyRead
#define CARDEB '['
#define CARFIN ']'
void ClientConnection::onReadyRead()
{
    int i, dep;
    QString dataRead;
    dataRead = _tcpSocket.readAll();

    if (!mSyncDeb || !mSyncFin) {
        // chercher le debut et la fin de la trame
        for (i=0 ; i<dataRead.size() ; i++) {
            if (dataRead[i]==CARDEB) {
                dep = i;
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Début de trame détecté !";
                mSyncDeb = true;
            } // if cardeb
            if (dataRead[i]==CARFIN && mSyncDeb) {
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Fin de trame détecté !";
                mSyncFin = true;
                _bufferDataRead = dataRead.mid(dep);  // on prend le début de trame incluant [
            } // if cardeb
        } // for i
    } else {
        _bufferDataRead += dataRead;  // on prend le reste de la trame
    } // else synchro

qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "_bufferDataRead :" << _bufferDataRead;

    if (_bufferDataRead.size() >= 12)  {
//        QRegExp lastValidFrameRegex("(" + regexFrameString + ")(?!" + regexFrameString + ")");
        QString regexFrameString("\\[[0-9A-Fa-f]{10}\\]");
        QRegExp validFrameRegex(regexFrameString);
        if(_bufferDataRead.left(12).contains(validFrameRegex)) {  // si trame bon format
            QString validFrameString = validFrameRegex.cap(0);
            _bufferDataRead = _bufferDataRead.mid(13); // ce qui reste
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "validFrameString :" << validFrameString;
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "il reste :" << _bufferDataRead;
            emit sig_frameReceived(validFrameString);
        } else {
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "!!! data.contains(validFrameRegex)";
            mSyncDeb = mSyncFin = false;
            emit sig_frameReceived("?"+_bufferDataRead.left(12));
        } //else
    } //if
}
*/

void CClientConnection::bleedBuffer()
{
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO;
    _tcpSocket.readAll();
}

void CClientConnection::on_disconnected()
{
    if (!_isAReader)
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "isNotAReader";
    else
    {
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "isAReader en deconnection";
//        QString nameDatabaseConnexion = QString::number(QThread::currentThreadId());

/*            _mutex.lock();
            QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
            _mutex.unlock();

            QSettings settings("SuPer.ini", QSettings::IniFormat);
            //prend valeur 1, si non trouver prend valeur 2
            QString ip = settings.value("Adresses/adresseSgbd", "192.168.2.70").toString();
            QString ndc = settings.value("Identifiants/NomDeCompte", "super2015").toString();
            QString mdp = settings.value("Identifiants/MotDePasse", "super2015").toString();
            QString NomBDD = settings.value("Identifiants/NomBase", "bdd_super2015").toString();
            db.setHostName(ip);
            db.setDatabaseName(NomBDD);
            db.setUserName(ndc);
            db.setPassword(mdp);

            if (!db.open())
            {
                qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "Error : QSqlDatabase::open() fail :" << db.lastError();
                emit sig_error("open database error");
            }
            else
            {

                QSqlQuery query(db);
                query.exec("UPDATE lecteur SET estConnecte=" + QString::number(false) + " WHERE ip=\"" + _clientAddress + "\";");
                if(!query.isActive())
                {
                    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [Update reader.isDisconnected] ERROR";
                    emit sig_error("sql error : [Update reader.isDisconnected]");
                }
                else
                {
                    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "QSqlQuery::exec() [Update lecteur.isDisconnected] ok";
                    query.finish();
                }
                db.close();
            }


        QSqlDatabase::removeDatabase(db.connectionName()); // TODO : Semble inutile, db est détruit au bloc précédent. L'enlever et réindenter ?
*/
        _bdd->setEtatLect(_clientAddress, false);
qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "[Update lecteur.isDisconnected] ok";

    }
}
