#ifndef CBADGE_H
#define CBADGE_H

#include <QObject>

class CBadge : public QObject
{
    Q_OBJECT
public:
    explicit CBadge(QObject *parent = nullptr);

    uint16_t getNumero() const;
    void setNumero(const uint16_t &value);
    uint16_t getMouvement() const;
    void setMouvement(const uint16_t &value);
    uint16_t getId() const;
    void setId(const uint16_t &value);
    bool getValide() const;
    void setValide(bool active);

private:
    uint16_t _numero;    // numéro interne du badge
    uint16_t _mouvement; // intensité du mouvement
    uint16_t _id;        // repérage sur le badge
    bool _pileFaible;     // pile faible
    bool _valide;         // bon params

signals:
    void sig_alertePileFaible(bool etat);

public slots:
};

#endif // CBADGE_H
