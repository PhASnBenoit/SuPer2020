#include <QApplication>
#include <QSettings>
#include "cihm.h"
#include "cserver.h"
#include "capp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /****RÉCUPERE IP ET PORT A PARTIR DUN FICHIER*************/
    QSettings MySetting("./SuPer.ini", QSettings::IniFormat);
    QString ip = MySetting.value("Adresses/adresseServ", "192.168.7.200").toString();
    QString port = MySetting.value("Ports/portServ", "2020").toString();
    /*********************************************************/

    CApp app(&a);
    CServer server(&a, ip, port);
    app.setServer(&server);
    CIhm ihm(nullptr, &app, &server);
    ihm.show();

    return a.exec();
}
