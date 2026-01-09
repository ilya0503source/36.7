#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QList>

// Поток для обработки одного клиента
class ClientHandler : public QThread {
    Q_OBJECT
public:
    ClientHandler(qintptr descriptor, QObject* parent = nullptr);
    void run() override;

signals:
    void messageReceived(const QString& msg, ClientHandler* sender);
    void disconnected(ClientHandler* sender);

public slots:
    void sendMessage(const QString& msg);

private:
    qintptr socketDescriptor;
    QTcpSocket* socket = nullptr;
};

// Основной сервер
class ChatServer : public QTcpServer {
    Q_OBJECT
public:
    explicit ChatServer(QObject* parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onClientMessage(const QString& msg, ClientHandler* sender);
    void onClientDisconnected(ClientHandler* sender);

private:
    QList<ClientHandler*> activeClients;
};

#endif // CHATSERVER_H
