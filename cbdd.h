#ifndef CBDD_H
#define CBDD_H

#include <QObject>
#include <QtSql>
#include <QList>
#include <QMessageBox>

#include "config.h"
// dynamique.h

//////////////////////////////////////////
/*** STRUCTURE pour retour de fonction***/
//////////////////////////////////////////
//identité personne
typedef struct s_Personne {
    QString nom;      //sauvegarde identité personne, nom
    QString prenom;   //prenom
    QString societe;  //societe
    QString email;
    QString portable;
} T_Personne;
//enregistrement d'un onglet
typedef struct s_TupleOnglet {
    int num_vue;
    QString legende;
    QString image;
} T_TupleOnglet;
//enregistrement d'un lecteur qui veut se déconnecter
typedef struct s_TupleLecteurS {
    int num_vue;
} T_TupleLecteurS;

//enregistrement d'un lecteur qui se connecte
typedef struct s_TupleLecteurE {
    int num_vue;
    int x;
    int y;
} T_TupleLecteurE;

//enregistrement des positions d'un lieu par rapport à une vue
typedef struct s_TuplePositionLieu {
    int xA;
    int yA;
    int xB;
    int yB;
} T_TuplePositionLieu;

//fichier log
typedef struct s_Log {
    QString date;
    QString legende;
    QString nom;
    QString prenom;
    QString societe;
} T_Log;

//badge existant
typedef struct s_Badge {
    int numBadge;
    QString id_badge;
} T_Badge;

//camera
typedef struct s_Camera {
    QString Adresse_IP;
    QString URL;
    QString Port_TCP;
    QString Type_Camera;
    QString Num_Camera;
    QString Num_Lieu;
    QString Num_Vue;
} T_Camera;

/////////////////////////////////////////////
/*** CLASSE pour gérer la Base de Données***/
class CBdd : public QObject
{
    Q_OBJECT
public:
    explicit CBdd(QObject *parent = nullptr);
    ~CBdd();

    bool _accesBdd;

    int getLecteur(QString adr, int *numLect, int *numLieu, int *typLect, QString *radr);
    int getNbVue();    //obtenir vue max
    bool getVueFctLect(int numZone, QList<T_TupleLecteurS> *listeVues); //obtenir la vue en fonction du lecteur
    int getLieuFctLecteur(int numLecteur, QString &legende); //obtenir le numéro du lieu en fonction du lecteur
    bool getVuePosFctLect(int numLecteur, QList<T_TupleLecteurE> *);   //obtenir la vue et la position (x,y) en fonction du lecteur
    bool getVues(QList<T_TupleOnglet>*);   //obtenir toutes les vues
    bool badgeExiste(int num_badge);   //est-ce que ce badge existe ?
    int badgeIdentite(int num_badge_i, QList<T_Personne> *listePersonne,QList<T_Badge> *listeBadge);   //a qui appartient ce badge ?
    //  bool getPositionLieu(int num_vue, int num_lecteur, QList<T_TuplePositionLieu> *); //obtenir (xA,yA) et (xB,yB) en fonction du lieu et de la vue

    bool getNomPersonne(int num_badge, QString *nom);
    int setOffTousLecteurs();
    void setEtatLect(int numLecteur, bool etat);    //connexion/déconnexion lecteur
    void setEtatLect(QString adrLecteur, bool etat);    //connexion/déconnexion lecteur
    bool getEtatLect(int numLecteur);       //état lecteur (connecté ou non)
    int getMilieuLecteur(int numLecteur);  // champs milieu du lecteur

    int getSensMonter(int numZone);     //obtenir le type de zone
    bool getPointsZone(int vue, int zone, T_Point *pointA, T_Point *pointB);  //obtenir les points de la zone

    //perte badge (réception)
    void setBadgeActif(int numBadge);    //mettre badge actif
    void setBadgePerdu(int numBadge);   //mettre badge perdu
    bool badgeExistant(QList<T_Badge> *);   //obtenir les badges actifs

    //gestion log
    void setLog(int typeLog, int numBadge);  //mettre à jour historique des événements

    //configuration SuPer
    void getTempo(int *, int *, int *tempoL);

    //Cameras
    bool getCameraExistante(QList<T_Camera> *);
    bool setChoixCamera(QString selectedCam);
    bool setInactif(QString inactif);
    int getNumCamera(QString camera);

private:
    //pointeurs sur structure
    T_TupleOnglet * pTupleOnglet;
    T_TupleLecteurS * pTupleLecteurS;
    T_TupleLecteurE * pTupleLecteurE;
    T_TuplePositionLieu * pTuplePositionLieu;
    T_Personne * pPersonne;
    T_Log * pLog;
    T_Badge * pBadge;
    T_Camera *pCamera;

    //pointeur sur la classe Dynamique (sauvegarde de l'affichage dynamique)
    //Dynamique *dynamique;

    QSqlQuery * query;
    QSqlDatabase  database;
    QString requete;  // requête parfois utilisé
    
signals:
    
public slots:
    
};

#endif // CBDD_H
