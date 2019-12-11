#ifndef CLABELBADGE_H
#define CLABELBADGE_H

#include <QObject>
#include <QTimer>
#include <QLabel>
#include <QTimer>
#include "cbadge.h"
#include "config.h"

class CLabelBadge : public CBadge
{
public:
    explicit CLabelBadge(QObject *parent = nullptr);
    void initTimers(uint16_t tempoM, uint16_t tempoR, uint16_t tempoL);

private:
    QTimer *_timerM;                   // mouvement nul > durée
    QTimer *_timerR;                    // 0 réception tous lecteurs > durée
    QTimer *_timerL[MAXLECTEURS];  // 0 réception d'un lecteur > durée

    QLabel *_labelB[MAXONGLETS];        // 1 QLabel / onglet
    uint16_t _memZone, _zone;          // zone-1 et zone en cours
    uint16_t _rssi[MAXLECTEURS][MAXVAL];   // rssi par lecteurs
    uint16_t _indRssi[MAXLECTEURS];        // indice _rssi
    uint16_t _moyRssi[MAXLECTEURS];        // moyenne _rssi
    uint16_t _memMoyRssi[MAXLECTEURS];     // moyenne _rssi-1
    int _tempoM, _tempoR, _tempoL; // valeur des timers pour un badge

    // coordonnées affichage
    T_Point _ptA,  // calcul du segment de droite
            _ptB, // calcul du segment de droite
            _ptBadge[MAXONGLETS]; // badge sur le segment
};

#endif // CLABELBADGE_H
