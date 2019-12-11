#include "cbadge.h"

CBadge::CBadge(QObject *parent) : QObject(parent)
{
    _pileFaible=false; // par défaut
    _mouvement = 0;    // par défaut
    _numero = 0x1000;  // au départ
    _id = 0x1000;      // au départ
    _valide = false;   // par défaut
}

uint16_t CBadge::getNumero() const
{
    return _numero;
}

void CBadge::setNumero(const uint16_t &value)
{
    _numero = value&0xFFF; // que 12 bits maxi
}

uint16_t CBadge::getMouvement() const
{
    return _mouvement;
}

void CBadge::setMouvement(const uint16_t &value)
{
    _mouvement = value&0xFFF; // que sur 12 bits
    if (_mouvement == 0xFFF) {// pile faible
        _pileFaible=true;
        emit sig_alertePileFaible(true);
    } else {
        _pileFaible = false;
        emit sig_alertePileFaible(true);
    } // else
}

uint16_t CBadge::getId() const
{
    return _id;
}

void CBadge::setId(const uint16_t &value)
{
    _id = value;
}

bool CBadge::getValide() const
{
    return _valide;
}

void CBadge::setValide(bool valide)
{
    _valide = valide;
}

