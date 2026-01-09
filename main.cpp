#include "mainwindow.h"
#include "startscreen.h"
#include "client.h"

#include <QApplication>
#include <QSettings>
#include <QFileInfo>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString appDir = QCoreApplication::applicationDirPath();
    QString configPath = appDir + "/config.ini";


    QFileInfo configInfo(configPath);
    if (!configInfo.exists()) {
        qCritical() << "Config file not found:" << configPath;
        return -1;
    }


    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup("Server");

    QString ip = settings.value("ip", "localhost").toString();
    int port = settings.value("port", 12345).toInt();
    settings.endGroup();


    Client client;
    client.connectToServer(ip, port);

    StartScreen startScreen(nullptr, &client);
    startScreen.show();

    return app.exec();
}
