#include "capp.h"

CApp::CApp(QObject *parent) : QObject(parent)
{

} // constructeur

CApp::~CApp()
{
    delete _bdd;
} // destructeur

int CApp::historiserAlarme(QString alarme)
{
    QFile log("logEvents.txt");

    // Ouverture / Création du fichier
    if(!log.exists())
    {
         bool test=log.open(QIODevice::WriteOnly | QIODevice::Text);
         if(!test)
         {
             QMessageBox::critical(nullptr,tr("Erreur lors de l'ouverture"),tr("Impossible d'ouvrir le fichier de log"));
             return 0;
         } // if test
    } else  {
         log.open(QIODevice::Append | QIODevice::Text);
    } // else exist
    //recuperation de la date
    QString heureActuelle = QTime::currentTime().toString();
    QString dateActuelle =QDate::currentDate().toString();
    //attribution du texte
    QTextStream flux(&log);
    //codage
    flux.setCodec("UTF-8");
    //historisation
    flux <<"Le "<< dateActuelle << " à "<< heureActuelle << ":"<<endl<< "       "<<alarme<<endl;
    log.close();
    return 1;
} // méthode

int CApp::ouvrirBdd()
{
    // accès BDD et initialisations
    _bdd=new CBdd();
    if(_bdd->_accesBdd == false){
        emit sig_erreurBdd(tr("Impossible d'etablir une connexion avec la Base de Donnees.\n"));
        return -1;
    } // if erreur bdd
    return 0;
}

int CApp::init()
{
    int r = ouvrirBdd();  // émet un message
    if (r) return r;

    _bdd->setOffTousLecteurs(); // aucun lecteur connecté
    // chargement des lecteurs
    // chargement des badges
    // chargement des tempos

    return 0;
}

CBdd *CApp::bdd() const
{
    return _bdd;
}

void CApp::startSupervision()
{
    //met le serveur en ecoute
    _server->switchOn();
}

void CApp::stopSupervision()
{
    //fermer toutes les connexions clients existantes
    _server->closeAllClientConnection();   // = clientConnection.close()
    _server->switchOff();  // = _server.close()
}

void CApp::on_startBackend()
{
    QFile log("Projet_Config");
    if(log.exists())
    {
        qDebug() << "Lancement du back-end";
        //lancer l'appli back-end
        if (!QProcess::startDetached("./Projet_Config"))
            qDebug() << "Erreur lancement back-end.";
    } else {
        emit sig_erreurCritique("Le logiciel de configuration est introuvable.");
    } // else


}

void CApp::setServer(CServer *server)
{
    _server = server;
} // méthode
