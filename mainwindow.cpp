#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "users.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget* parent, Client* client, const QString& login)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_client(client), m_login(login)
{
    ui->setupUi(this);
    if (m_client) {
        connect(m_client, &Client::messageReceived,
                this, &MainWindow::onMessageReceived);
        QString message = "showmessages;";
        m_client->sendMessage(message);
    }
    connect(m_client, &Client::showMessagesResponse,
            this, &MainWindow::onMessageReceived);
    connect(m_client, &Client::disconnectUserResponse,
            this, &MainWindow::onDisconnectUserResponse);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_returnPressed()
{
    on_sendMessageButton_clicked();
}

void MainWindow::on_sendMessageButton_clicked()
{
    QString message = "sendmessage;" + m_login + ";all;" + ui->lineEdit->text();
    m_client->sendMessage(message);
}

void MainWindow::on_privateMessageSendButton_clicked()
{
    Users* userList = new Users(nullptr,m_client,m_login, ui->lineEdit->text());
    userList->show();
}

void MainWindow::onMessageReceived(const QString& rawMessages)
{
    QStringList messageList = rawMessages.split("|", Qt::SkipEmptyParts);
    for (const QString& rawMsg : messageList) {
        QStringList msgParts = rawMsg.split(";", Qt::SkipEmptyParts);
        if (msgParts.size() != 3) {
            qDebug() << "MainWindow: invalid message format:" << rawMsg;
            continue;
        }
        QString sender = msgParts[0];
        QString recipient = msgParts[1];
        QString text = msgParts[2];
        QString formatted = QString("[%1] → [%2] : %3")
                                .arg(sender)
                                .arg(recipient)
                                .arg(text);
        if (sender == "all" || recipient == "all") {
            ui->commonChatBrowser->append(formatted);
        } else if (sender == m_login || recipient == m_login) {
            ui->privateChatBrowser->append(formatted);
        } else {
            qDebug() << "MainWindow: message skipped:" << formatted;
        }
    }
}

void MainWindow::onDisconnectUserResponse(){
   QMessageBox::information(this, "Отключение", "Вы отключены от сервера!");
    this-> close();
}
