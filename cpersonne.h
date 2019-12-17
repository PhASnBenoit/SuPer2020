#ifndef CPERSONNE_H
#define CPERSONNE_H

#include <QObject>

class CPersonne : public QObject
{
    Q_OBJECT
public:
    explicit CPersonne(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CPERSONNE_H