#include "chatDB.h"
#include <QDebug>
#include <QSqlError>

QSqlDatabase ChatDB::m_db;

bool ChatDB::initializeDatabase() {
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setDatabaseName("MySQL_Qt");
    m_db.setUserName("root");
    m_db.setPassword("root");

    if (!m_db.open()) {
        LOG("Не удалось подключиться к серверу MySQL: " + m_db.lastError().text());
        return false;
    }
    LOG("Подключение к серверу MySQL выполенено успешно");

    if (!createDatabaseIfNotExists()) {
        LOG("Не удалось создать базу данных");
        m_db.close();
        return false;
    }

    QSqlQuery query(m_db);
    if (!query.exec("USE chat")) {
        LOG("Не удалось выбрать базу данных 'chat': " + query.lastError().text());
        m_db.close();
        return false;
    }
    LOG("База данных chat выбрана успешно");

    if (!createTablesIfNotExists()) {
        LOG("Не удалось создать таблицы");
        m_db.close();
        return false;
    }

    return true;
}

bool ChatDB::createDatabaseIfNotExists() {
    QSqlQuery query(m_db);

    query.exec("SHOW DATABASES LIKE 'chat'");
    if (query.next()) {
        LOG("База данных chat уже существует");
        return true;
    }

    if (!query.exec("CREATE DATABASE chat")) {
        LOG("Не удалось создать базу данных chat: " + query.lastError().text());
        return false;
    }
    LOG("База данных chat успешно создана");
    return true;
}

bool ChatDB::createTablesIfNotExists() {
    QSqlQuery query(m_db);

    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            login VARCHAR(50) PRIMARY KEY,
            password VARCHAR(64) NOT NULL,
            banStatus BOOLEAN NOT NULL DEFAULT FALSE
        )
    )";
    if (!query.exec(createUsersTable)) {
        LOG("Не удалось создать таблицу users: " + query.lastError().text());
        return false;
    }

    QString createMessagesTable = R"(
        CREATE TABLE IF NOT EXISTS messages (
            sender VARCHAR(50) NOT NULL,
            recipient VARCHAR(50) NOT NULL,
            text TEXT NOT NULL
        )
    )";
    if (!query.exec(createMessagesTable)) {
        LOG("Не удалось создать таблицу messages: " + query.lastError().text());
        return false;
    }

    return true;
}

bool ChatDB::loadUsers(QVector<User>& users) {
    QSqlQuery query(m_db);
    if (!query.exec("SELECT login, password, banStatus FROM users")) {
        LOG("Ошибка загрузки таблицы users: " + query.lastError().text());
        return false;
    }

    while (query.next()) {
        QString login = query.value(0).toString();
        QString password = query.value(1).toString();
        bool banStatus = query.value(2).toBool();
        users.append(User(login, password, banStatus));
    }
    return true;
}

bool ChatDB::loadMessages(QVector<Message>& messages) {
    QSqlQuery query(m_db);
    if (!query.exec("SELECT sender, recipient, text FROM messages")) {
        LOG("Ошибка загрузки таблицы messages: " + query.lastError().text());
        return false;
    }

    while (query.next()) {
        QString sender = query.value(0).toString();
        QString recipient = query.value(1).toString();
        QString text = query.value(2).toString();
        messages.append(Message(sender, recipient, text));
    }
    return true;
}

bool ChatDB::addUserToDB(const User& user) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (login, password, banStatus) VALUES (?, ?, ?)");
    query.addBindValue(user.getLogin());
    query.addBindValue(user.getPassword());
    query.addBindValue(user.getBanStatus());

    if (!query.exec()) {
        LOG("Ошибка добавления пользователя в базу данных: " + query.lastError().text());
        return false;
    }
    return true;
}

bool ChatDB::addMessageToDB(const Message& message) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO messages (sender, recipient, text) VALUES (?, ?, ?)");
    query.addBindValue(message.getSender());
    query.addBindValue(message.getRecipient());
    query.addBindValue(message.getText());

    if (!query.exec()) {
        LOG("Ошибка добавления сообщения в базу данных: " + query.lastError().text());
        return false;
    }
    return true;
}

void ChatDB::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
        LOG("Соединение с базой данных закрыто");
    }
}
