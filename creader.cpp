#include "creader.h"

CReader::CReader()
{
    _number = 0;
    _placeId = 0;
    _address = "";
    _isConnected = false;
}

/*Reader::Reader(int number, int placeId, QString address, bool isConnected)
{
    _number = number;
    _placeId = placeId;
    _address = address;
    _isConnected = isConnected;
}*/

CReader::CReader(const CReader& reader)
{
    _number = reader._number;
    _placeId = reader._placeId;
    _address = reader._address;
    _isConnected = reader._isConnected;
}

CReader::~CReader()
{

}

int CReader::number() const
{
    return _number;
}

int CReader::placeId() const
{
    return _placeId;
}

QString CReader::address() const
{
    return _address;
}

bool CReader::isConnected() const
{
    return _isConnected;
}

void CReader::number(int number)
{
    _number = number;
}

void CReader::placeId(int placeId)
{
    _placeId = placeId;
}

void CReader::address(QString address)
{
    _address = address;
}

void CReader::setConnected(bool isConnected)
{
    _isConnected = isConnected;
}

