#include "client.h"
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QCryptographicHash>

Client::Client(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
}

QString Client::hashPassword(const QString& password) {
    QByteArray data = password.toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex();
}

Client::~Client()
{
    disconnectFromServer();
}

void Client::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

void Client::disconnectFromServer()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool Client::isConnected() const
{
    return m_socket->state() == QTcpSocket::ConnectedState;
}

void Client::sendMessage(const QString &message)
{
    if (isConnected()) {
        QByteArray data = message.toUtf8();
        m_socket->write(data);
        m_socket->flush();
    } else {
        emit errorOccurred("Not connected to server");
    }
}

void Client::onReadyRead() {
    QByteArray data = m_socket->readAll();
    if (data.isEmpty()) return;

    QString message = QString::fromUtf8(data);
    QStringList parts = message.split(";", Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    QString command = parts[0];

    if (command == "login") {
        if (parts.size() > 1) {
            QString lognResponse = parts.mid(1).join(";");
            qDebug() << lognResponse;
            emit loginResponse(lognResponse);
        }
    } else if (command == "registration") {
        if (parts.size() > 1) {
            QString regResponse = parts.mid(1).join(";");
            emit registrationResponse(regResponse);
        }
    } else if (command == "showusers") {
        if (parts.size() > 1) {
            QString usersList = parts.mid(1).join(";");
            emit showUsersResponse(usersList);
        }
    } else if (command == "showmessages") {
        if (parts.size() > 1) {
            QString messagesData = parts.mid(1).join(";");
            QStringList individualMessages = messagesData.split("|", Qt::SkipEmptyParts);
            QString combinedMessages = individualMessages.join("|");
            emit showMessagesResponse(combinedMessages);
        }
    } else if (command == "disconnectuser") {
        m_socket->close();
        emit disconnectUserResponse();
    } else {
        qDebug() << "Client: unknown command:" << command << "in message:" << message;
    }
}

void Client::onConnected()
{
    emit connected();
}

void Client::onDisconnected()
{
    emit disconnected();
}

void Client::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit errorOccurred(m_socket->errorString());
}
