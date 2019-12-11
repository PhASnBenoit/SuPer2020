#include "clabelbadge.h"

CLabelBadge::CLabelBadge(QObject *parent)
    :CBadge(parent)
{

}

void CLabelBadge::initTimers(uint16_t tempoM, uint16_t tempoR, uint16_t tempoL)
{
    _tempoM = tempoM;
    _tempoR = tempoR;
    _tempoL = tempoL;
}
