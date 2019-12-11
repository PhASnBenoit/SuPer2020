#ifndef CREADER_H
#define CREADER_H

#include <QString>
#include <QMetaType>

class CReader
{
public:
    CReader();
    //Reader(int=0, int=0, QString="", bool=false);
    CReader(const CReader&);
    ~CReader();

    int number() const;
    int placeId() const;
    QString address() const;
    bool isConnected() const;

    void number(int);
    void placeId(int);
    void address(QString);
    void setConnected(bool);

private:
    int _number;
    int _placeId;
    QString _address;
    bool _isConnected;
};

Q_DECLARE_METATYPE(CReader)

#endif // CREADER_H
