#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr, Client* client = nullptr, const QString& login = "");
    ~MainWindow();

private slots:
    void on_lineEdit_returnPressed();
    void on_sendMessageButton_clicked();
    void on_privateMessageSendButton_clicked();
    void onMessageReceived(const QString& message);
    void onDisconnectUserResponse();

private:
    Ui::MainWindow *ui;
    Client* m_client;
    QString m_login;
};
#endif // MAINWINDOW_H
