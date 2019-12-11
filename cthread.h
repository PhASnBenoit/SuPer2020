#ifndef CTHREAD_H
#define CTHREAD_H

#include <QThread>
#include <QDebug>

class CThread : public QThread
{
Q_OBJECT
public:
    explicit CThread(QObject *parent = nullptr);
    ~CThread();
    void run();

private slots:
    void slot_started();
    void slot_finished();
    void slot_destroyed();
    //void slot_terminated();
};

#endif // CTHREAD_H
