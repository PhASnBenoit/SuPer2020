#include "cihm.h"
#include "ui_cihm.h"

CIhm::CIhm(QWidget *parent, CApp *app, CServer *server) :
    QMainWindow(parent),
    ui(new Ui::CIhm),
    _server(server),
    _app(app)
{
    ui->setupUi(this);
    //Ajouter deux menus
    menuFichier = menuBar()->addMenu("&Fichier");
    menuEdition = menuBar()->addMenu("&Edition");
    actionOn = new QAction("&Lancer la supervision",this);
    actionOn->setCheckable(true);
    connect(actionOn, &QAction::triggered, this, &CIhm::on_startSupervision);
    menuFichier->addAction(actionOn);
    actionOff = new QAction("&Arrêter la supervision",this);
    connect(actionOff, &QAction::triggered, this, &CIhm::on_stopSupervision);
    menuFichier->addAction(actionOff);
    actionQuitter = new QAction("&Quitter",this);
    connect(actionQuitter, &QAction::triggered, this, &CIhm::close);
    menuFichier->addAction(actionQuitter);
    actionBackend = new QAction("&Lancer Configuration",this);
    connect(actionBackend, &QAction::triggered, _app, &CApp::on_startBackend);
    menuEdition->addAction(actionBackend);
    tmrDate = new QTimer();
    connect(tmrDate,&QTimer::timeout,this,&CIhm::on_timerDateHeure);
    tmrDate->start(1000);
    connect(ui->btClear, &QPushButton::clicked, ui->txtAlarme, &QTextEdit::clear);

    // Affichage des erreurs d'ouverture de la BDD
    connect(_app, &CApp::sig_erreurBdd, this, &CIhm::on_erreurBdd);
    connect(_app, &CApp::sig_erreurCritique, this, &CIhm::on_erreurCritique);

    _app->init();  // init bdd, etc...
    _bdd = _app->bdd();
    _server->setBdd(_bdd);

    int vueMax = afficherOnglets();
    //supprimer les deux onglets de base
    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(vueMax);
    // Afficher l'onglet principal
    ui->tabWidget->setCurrentIndex(0);

    // Démarrage de la supervision
    _app->startSupervision();
} // méthode

CIhm::~CIhm()
{
    for(int i=0 ; _tabLabelBadges.size() ; i++) {
        delete _tabLabelBadges.at(i);
    } // for
    int sz = _tabOnglets.size();
    for(int i=0 ; i<sz ; i++) {
        delete _tabOnglets.at(i);
    } // for
    delete actionOn;
    delete actionOff;
    delete actionQuitter;
    delete actionBackend;
    delete tmrDate;
    delete ui;
}  // méthode

void CIhm::ajoutLecteur(int numLecteur, QString addLecteur, int numLieu,
                        QString legende, int num_vue, int x, int y, CClientConnection *cCL)
{
    QLabel *laOnglet;
    laOnglet = _tabOnglets.at(num_vue);

    //nouveau label dynamique pour mettre l'image correspondant
    QLabel *labelL = new QLabel(laOnglet);
    QString numLecteurE = QString::number(numLecteur);
    if(num_vue == 1){
        labelL->setPixmap(QPixmap("ressources/lecteur_actif_petit.jpg"));
    }else{
        labelL->setPixmap(QPixmap("ressources/lecteur_actif.jpg"));
    }
    labelL->setGeometry(x, y, 15, 42); // largeur hauteur à définir
    labelL->setVisible(true);

    //affichage informations lecteur
    labelL->setToolTip("Lecteur n° "+ numLecteurE+" actif.\n"
                       +"Adresse IP : "+ addLecteur+"\n"
                       +"Lieu n° "+ QString::number(numLieu)
                       +" : "+legende);

    //message d'avertissement (Alarmes)
    if(num_vue == 1)
    {
        QString ajLecteur("<Lecteur " + numLecteurE + QString::fromUtf8(" > vient de se connecter."));
        qDebug() << ajLecteur;
        //obtenir date
        QString date = QDateTime::currentDateTime().toString();
        ui->txtAlarme->append(date+ajLecteur + "\n");

        //historiser l'evenement
        QString alarme = "Lecteur n° : ";
        alarme+= numLecteurE ;
        alarme+= " ajouté.";
        _app->historiserAlarme(alarme);
    }

    //en cas de suppression
    connect(cCL, SIGNAL(sig_disconnected()), labelL, SLOT(clear()));
    connect(cCL, SIGNAL(sig_disconnected()), labelL, SLOT(deleteLater()));

} // méthode


int CIhm::afficherOnglets()
{
    int vueMax = _bdd->getNbVue();
    QList<T_TupleOnglet> listeTupleO;
    _bdd->getVues(&listeTupleO);
    if(!listeTupleO.empty()){
        for(int i = 0; i < listeTupleO.count() && i < vueMax; i++) {
            QLabel *laImageOnglet = new QLabel(this);
            laImageOnglet->setPixmap(QPixmap(listeTupleO.at(i).image));
            ui->tabWidget->insertTab(listeTupleO.at(i).num_vue, laImageOnglet, listeTupleO.at(i).legende);
            _tabOnglets.append(laImageOnglet);
        } // for
    } // if empty
    return vueMax;
} // méthode

void CIhm::on_erreurBdd(QString mess)
{
    QMessageBox::critical(nullptr, mess, mess, QMessageBox::Cancel);
    this->destroy();
}

void CIhm::on_erreurCritique(QString mess)
{
    QMessageBox::critical(nullptr, mess, mess, QMessageBox::Cancel);
} // méthode

void CIhm::on_startSupervision()
{
    //checker le bouton on pour indiquer que le serveur est lancé
    actionOn->setChecked(true);
    //bouton nettoyer autorisé
    ui->btClear->setEnabled(true);
    _app->startSupervision();
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "start SUPERVISION :" ;
    //activer la premiere caméras disponible
    //on_comboBox_activated(0);
}

void CIhm::on_stopSupervision()
{
    //dechecker le bouton on pour indiquer que le serveur est désactivé
    actionOn->setChecked(false);
    //fermer le serveur
    _app->stopSupervision();
    //Afficher le débug
    qDebug() << QThread::currentThreadId() << Q_FUNC_INFO << "stop SUPERVISION :";
}

void CIhm::on_timerDateHeure()
{
    //Récuperation de la date actuelle
    QString heureActuelle = QTime::currentTime().toString();
    QString dateActuelle = QDate::currentDate().toString();
    //création de la chaine finale
    QString date(dateActuelle+" "+heureActuelle);
    //Affectation de la chaine finale dans un label
    ui->lbDate->setText(date);
} //méthode
