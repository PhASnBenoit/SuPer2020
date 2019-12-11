#ifndef CAPP_H
#define CAPP_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTime>
#include <QDate>
#include <QMessageBox>
#include <QTextStream>
#include "cbdd.h"
#include "cserver.h"
#include "clabelbadge.h"

class CApp : public QObject
{
    Q_OBJECT
public:
    explicit CApp(QObject *parent = nullptr);
    ~CApp();
    int historiserAlarme(QString alarme);
    int ouvrirBdd();
    int init();
    CBdd *bdd() const;
    void startSupervision();
    void stopSupervision();
    void setServer(CServer *server);

public slots:
    void on_startBackend();

private:
    CBdd *_bdd;
    CServer *_server;
    QList<CBadge *> _tabBadges;

signals:
    void sig_erreurBdd(QString mess);
    void sig_erreurCritique(QString mess);

public slots:
};

#endif // CAPP_H
