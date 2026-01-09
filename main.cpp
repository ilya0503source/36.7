#include "mainwindow.h"
#include "chatDB.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!ChatDB::initializeDatabase()) {
        return -1;
    }

    MainWindow w;
    w.show();

    QObject::connect(&a, &QCoreApplication::aboutToQuit, [] {
        ChatDB::closeDatabase();
    });
    return a.exec();
}
