#ifndef CONFIG_H
#define CONFIG_H

#define MAXONGLETS 10   //maximum d'onglets possible
#define MAXLECTEURS 10  //maximum de lecteurs possible (NB : 1 lieu = 1 lecteur)
#define MAXVAL 20       //nombre de valeur pour faire la moyenne
#define MAXBADGES 10    //maximum de badges possible
#define MAXPERSONNES 10 //maximum de personnes possible

// structure pour le positionnement X, Y du label badge
typedef struct Point {
    int x,y;
} T_Point;

#endif // CONFIG_H
