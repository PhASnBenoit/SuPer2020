#ifndef CLECTEURRFID_H
#define CLECTEURRFID_H

#include <QObject>
#include <QHostAddress>

class CLecteurRfid : public QObject
{
    Q_OBJECT
public:
    explicit CLecteurRfid(QObject *parent = nullptr);

private:
    u_int16_t numero;   // numéro interne du lecteur
    QHostAddress adresseIp;

signals:

public slots:
};

#endif // CLECTEURRFID_H
