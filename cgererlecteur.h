#ifndef CCLIENTCONNECTION_H
#define CCLIENTCONNECTION_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QMetaType>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegExp>
#include <QDebug>
#include <QSettings>

#include "ctcpsocket.h"
#include "creader.h"
#include "cbdd.h"

class CClientConnection : public QObject
{
Q_OBJECT
signals:
    void sig_isAReader(CReader);
    void sig_isNotAReader(QString);
    void sig_frameReceived(QString);
    void sig_disconnected();
    void sig_closed(); // TODO : Inutile car en cas de rupture de connexion disconnected est aussi émis. A supprimer ?
    void sig_error(QString);

public:
    explicit CClientConnection(qintptr sd = 0, CBdd *bdd = nullptr);
    ~CClientConnection(); // TODO : Personne ne devrait l'employer, le rendre privé ?

public slots:
    void open(); // TODO : Seul server (ou le thread ?) l'emploient, la rendre privée et ajouter server en amis ?
    void close();
    QString getClientAddress();

private:
    CTcpSocket _tcpSocket;
    qintptr _socketDescriptor;
    bool _opened;
    bool _SyncDeb, _SyncFin;
    static QMutex _mutex;
    bool _isAReader;
    QString _clientAddress;
    QString _bufferDataRead;
    CBdd *_bdd;

private slots:
    void filter(); // N'est connecté nulle part. N'a pas à être un slot ?
    void onReadyRead();
    void bleedBuffer();
    void on_disconnected();
};

#endif // CCLIENTCONNECTION_H
