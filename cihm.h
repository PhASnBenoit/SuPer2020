#ifndef CIHM_H
#define CIHM_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include "cbdd.h"
#include "cserver.h"
#include "clabelbadge.h"
#include "capp.h"

namespace Ui {
class CIhm;
}

class CIhm : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhm(QWidget *parent = nullptr, CApp *app = nullptr, CServer *server = nullptr);
    ~CIhm();

private:
    Ui::CIhm *ui;
    QList<QLabel *> _tabOnglets;
    QList<CLabelBadge *> _tabLabelBadges;
    CBdd *_bdd;
    CServer *_server;
    CApp *_app;
    //MENUBAR
    QMenu *menuFichier;
    QMenu *menuEdition;
    QAction *actionQuitter;
    QAction *actionBackend;
    QAction *actionOn;
    QAction *actionOff;

    QTimer *tmrDate;

    void ajoutLecteur(int numLecteur, QString addLecteur, int numLieu, QString legende,
                      int num_vue, int x, int y, CClientConnection *cCL);
    int afficherOnglets();
    void sltActualiserDate();

private slots:
    void on_erreurBdd(QString mess);
    void on_erreurCritique(QString mess);
    void on_startSupervision();
    void on_stopSupervision();
    void on_timerDateHeure();
};

#endif // CIHM_H
