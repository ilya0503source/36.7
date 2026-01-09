#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;
    void sendMessage(const QString &message);
    QString hashPassword(const QString& password);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &message);
    void errorOccurred(const QString &error);
    void loginResponse(const QString& response);
    void registrationResponse(const QString& response);
    void showUsersResponse(const QString& users);
    void showMessagesResponse(const QString& messages);
    void disconnectUserResponse();

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;
};

#endif // CLIENT_H
