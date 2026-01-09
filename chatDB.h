#ifndef CHATDB_H
#define CHATDB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include <QSqlError>
#include <QVector>
#include "user.h"
#include "message.h"
#include "logger.h"

class ChatDB {
public:
    static bool initializeDatabase();
    static bool loadUsers(QVector<User>& users);
    static bool loadMessages(QVector<Message>& messages);
    static bool addUserToDB(const User& user);
    static bool addMessageToDB(const Message& message);
    static void closeDatabase();

private:
    static QSqlDatabase m_db;
    static bool createDatabaseIfNotExists();
    static bool createTablesIfNotExists();
};

#endif // CHATDB_H
