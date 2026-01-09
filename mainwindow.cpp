#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverChat.h"
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_server(nullptr)
{
    ui->setupUi(this);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->menu_stop->setEnabled(false);
    connect(Logger::instance(), &Logger::logMessage, this, &MainWindow::onLogMessage);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested,
            this, &MainWindow::showUserContextMenu);
    connect(ui->menu_stop, &QAction::triggered,
                this, &MainWindow::handleMenuStop);
}

MainWindow::~MainWindow()
{
    if (m_server) {
        m_server->deleteLater();
    }
    delete ui;
}

void MainWindow::on_menu_start_triggered()
{
    ui->menu_stop->setEnabled(true);

    if (m_server) {
        LOG("Сервер уже запущен");
        return;
    }

    m_server = new ServerChat(this);

    connect(m_server, &ServerChat::userListChanged, this, &MainWindow::updateUsersList);
    connect(m_server, &ServerChat::messageReceived, this, &MainWindow::updateMessagesDisplay);
    connect(m_server, &ServerChat::clientHandlerCreated, [this](ClientHandler* handler) {
        connect(handler, &ClientHandler::messageReceived, this, &MainWindow::updateMessagesDisplay);
    });
    connect(m_server, &ServerChat::clientHandlerCreated, [this](ClientHandler* handler) {
        connect(handler, &ClientHandler::userListChanged, this, &MainWindow::updateUsersList);
    });
    connect(m_server, &ServerChat::clientHandlerCreated, [this](ClientHandler* handler) {
        connect(handler, &ClientHandler::messageReceived, this, &MainWindow::updateMessagesDisplay);
    });

    m_server->getUsers().append(User("admin", "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918", false));

    updateUsersList();
    updateMessagesDisplay();

    if (m_server->startServer(12345)) {
    LOG("Сервер запущен");
    } else {
        LOG("ошибка запуска сервера: "+ m_server->errorString());
        delete m_server;
        m_server = nullptr;
    }
}

void MainWindow::updateUsersList()
{
    if (!m_server) return;

    ui->listWidget->clear();
    m_userItems.clear();

    const QVector<User>& users = m_server->getUsers();
    for (const User& user : users) {
        QListWidgetItem* item = new QListWidgetItem(user.getLogin());

        if (user.getBanStatus()) {
            item->setForeground(Qt::red);
        } else {
            item->setForeground(Qt::black);
        }
        ui->listWidget->addItem(item);
        m_userItems[user.getLogin()] = item;
    }
}

void MainWindow::showUserContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = ui->listWidget->itemAt(pos);
    if (!item) return;
    QString login = item->text();
    QVector<User>& users = m_server->getUsers();
    auto it = std::find_if(users.begin(), users.end(),
                           [&login](const User& user) {
                               return user.getLogin() == login;
                           });
    if (it == users.end()) return;

    QMenu menu;
    if (!it->getBanStatus()) {
        QAction* banAction = menu.addAction("Забанить");
        connect(banAction, &QAction::triggered, [this, login]() {
            banUser(login);
            LOG("Пользователь "+ login + " забанен");
        });        
    }
    if (it->getBanStatus()) {
        QAction* unbanAction = menu.addAction("Разбанить");
        connect(unbanAction, &QAction::triggered, [this, login]() {
            unbanUser(login);
            LOG("Пользователь "+ login + " разбанен");
        });        
    }
    QAction* disconnectAction = menu.addAction("Отключить");
    connect(disconnectAction, &QAction::triggered, [this, login]() {
        disconnectUser(login);
        LOG("Пользователь "+ login + " отключен");
    });
    menu.exec(ui->listWidget->mapToGlobal(pos));
}

void MainWindow::banUser(const QString& login)
{
    if (!m_server) return;

    QVector<User>& users = m_server->getUsers();
    auto it = std::find_if(users.begin(), users.end(),
                           [&login](const User& user) {
                               return user.getLogin() == login;
                           });
    if (it != users.end()) {
        it->setBanStatus(true);
        disconnectUser(login);
        updateUsersList();
    }
}

void MainWindow::unbanUser(const QString& login)
{
    if (!m_server) return;
    QVector<User>& users = m_server->getUsers();
    auto it = std::find_if(users.begin(), users.end(),
                           [&login](const User& user) {
                               return user.getLogin() == login;
                           });
    if (it != users.end()) {
        it->setBanStatus(false);
        updateUsersList();
    }
}


void MainWindow::disconnectUser(const QString& login)
{
    if (!m_server) return;
    m_server->disconnectUser(login);
}


void MainWindow::updateMessagesDisplay()
{
    if (!m_server) return;
    ui->textEdit->clear();
    const QVector<Message>& messages = m_server->getMessages();
    for (const Message& msg : messages) {
        QString line = QString("[%1] -> [%2] : %3")
        .arg(msg.getSender())
            .arg(msg.getRecipient())
            .arg(msg.getText());
        ui->textEdit->append(line);
    }
}

void MainWindow::handleMenuStop()
{
    static std::atomic<bool> isShuttingDown(false);
    if (isShuttingDown.exchange(true)) {
        return;
    }
    ui->menu_stop->setEnabled(false);
    if (!m_server) {
        QMessageBox::information(this, "Статус", "Сервер не запущен.");
        isShuttingDown.store(false);
        return;
    }
    const QVector<ClientHandler*> activeClients = m_server->getActiveClients();
    LOG("Инициирование отключения для активных соединений: " + activeClients.size());
    for (ClientHandler* client : activeClients) {
        if (client) {
            client->disconnectClient();
        }
    }
    m_server->close();
    m_server->deleteLater();
    m_server = nullptr;
    isShuttingDown.store(false);
    LOG("Сервер остановлен");
    QApplication::quit();
}

void MainWindow::onLogMessage(const QString& message) {
    statusBar()->showMessage(message, 5000);
}
