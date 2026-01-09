#include "users.h"
#include "ui_users.h"

Users::Users(QWidget *parent, Client* client, const QString& login, const QString& text)
    : QDialog(parent)
    , ui(new Ui::Users)
{
    ui->setupUi(this);
    m_client = client;
    m_login = login;
    m_text = text;
    QString message = "showusers;";
    m_client->sendMessage(message);

    connect(client, &Client::showUsersResponse,
            this, &Users::onShowUsersResponse);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked,
            this, &Users::onListWidgetDoubleClicked);
}

Users::~Users()
{
    delete ui;
}


void Users::onShowUsersResponse(const QString& response) {
    QStringList users = response.split(";", Qt::SkipEmptyParts);
    ui->listWidget->clear();
    ui->listWidget->addItems(users);
}

void Users::onListWidgetDoubleClicked(QListWidgetItem* item) {
    if (item) {
        m_selectedUser = item->text();
        QString message = "sendmessage;" + m_login + ";" + m_selectedUser + ";" + m_text;
        m_client->sendMessage(message);
        this->close();
    }
}
