#ifndef SERVERCHAT_H
#define SERVERCHAT_H

#include <QString>
#include <QVector>
#include "user.h"
#include "message.h"
#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include "logger.h"

class ServerChat;

class ClientHandler : public QThread
{
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket* socket,
                           QVector<User>& users,
                           QVector<Message>& messages,
                           QMutex& usersMutex,
                           QMutex& messagesMutex,
                           ServerChat* server,
                           QObject* parent = nullptr

                           );
    void run() override;
    QString getLogin() const { return m_login; }
    void disconnectClient();
    void sendSingleMessageToClient(const Message& msg);

signals:
    void clientDisconnected();
    void userListChanged();
    void messageReceived(const Message& msg);

private:
    QTcpSocket* m_socket;
    QVector<User>& m_users;
    QVector<Message>& m_messages;
    QString m_login;
    QMutex& m_usersMutex;
    QMutex& m_messagesMutex;
    ServerChat* m_server;
    void sendMessagesToClient();
};

class ServerChat : public QTcpServer
{
    Q_OBJECT
public:
    explicit ServerChat(QObject* parent = nullptr);
    bool startServer(quint16 port);
    QVector<User>& getUsers() { return m_users; }
    QVector<Message>& getMessages() { return m_messages; }
    QMutex& getUsersMutex() { return m_usersMutex; }
    QMutex& getMessagesMutex() { return m_messagesMutex; }
    void disconnectUser(const QString& login);
    void broadcastNewMessage(const Message& msg);
    bool isUserConnected(const QString& login) const;
    QVector<ClientHandler*> getActiveClients();
    QMutex& getDbMutex() { return m_dbMutex; }

signals:
    void clientHandlerCreated(ClientHandler* handler);
    void userListChanged();
    void messageReceived(const Message& msg);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QList<ClientHandler*> m_clientHandlers;
    QVector<User> m_users;
    QVector<Message> m_messages;
    QMutex m_usersMutex;
    QMutex m_messagesMutex;
    QMutex m_dbMutex;
};

#endif // SERVERCHAT_H
