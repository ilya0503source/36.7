#ifndef USERS_H
#define USERS_H

#include <QDialog>
#include <QListWidgetItem>
#include "client.h"
#include "mainwindow.h"

namespace Ui {
class Users;
}

class Users : public QDialog
{
    Q_OBJECT

public:
    explicit Users(QWidget* parent = nullptr, Client* client = nullptr, const QString& login = "", const QString& text = "");
    ~Users();

public slots:
    void onShowUsersResponse(const QString& response);
private slots:
    void onListWidgetDoubleClicked(QListWidgetItem* item);

private:
    Ui::Users *ui;
    QString m_selectedUser;
    Client* m_client;
    QString m_login;
    QString m_text;
};

#endif // USERS_H
