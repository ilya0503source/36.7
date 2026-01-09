#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "serverChat.h"
#include "logger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_menu_start_triggered();
    void updateUsersList();
    void showUserContextMenu(const QPoint& pos);
    void updateMessagesDisplay();
    void handleMenuStop();
    void onLogMessage(const QString& message);

private:
    Ui::MainWindow *ui;
    ServerChat* m_server;
    void banUser(const QString& login);
    void unbanUser(const QString& login);
    void disconnectUser(const QString& login);
    QMap<QString, QListWidgetItem*> m_userItems;
};

#endif // MAINWINDOW_H
