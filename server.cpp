#include "server.h"
#include <QDataStream>
#include <QDebug>

// === Реализация ClientHandler ===
ClientHandler::ClientHandler(qintptr descriptor, QObject* parent)
    : QThread(parent), socketDescriptor(descriptor) {}

void ClientHandler::run() {
    socket = new QTcpSocket();
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    // Цикл приёма сообщений от клиента
    while (socket->state() == QAbstractSocket::ConnectedState) {
        socket->waitForReadyRead(1000);  // Ждать до 1 сек


        if (socket->bytesAvailable() > 0) {
            QDataStream in(socket);
            in.setVersion(QDataStream::Qt_6_5);
            QString message;
            in >> message;
            emit messageReceived(message, this);  // Передаём сообщение + указатель на себя
        }
    }

    emit disconnected(this);  // Сообщаем об отключении (с указанием себя)
    socket->deleteLater();
}

void ClientHandler::sendMessage(const QString& msg) {
    if (!socket || socket->state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << msg;

    socket->write(block);
    socket->waitForBytesWritten(1000);
}

// === Реализация ChatServer ===
ChatServer::ChatServer(QObject* parent)
    : QTcpServer(parent) {}


void ChatServer::incomingConnection(qintptr socketDescriptor) {
    ClientHandler* handler = new ClientHandler(socketDescriptor, this);

    // Подключаем сигналы потока к слотам сервера
    connect(handler, &ClientHandler::messageReceived,
            this, &ChatServer::onClientMessage);
    connect(handler, &ClientHandler::disconnected,
            this, &ChatServer::onClientDisconnected);

    activeClients.append(handler);  // Добавляем в список активных
    handler->start();                // Запускаем поток


    qDebug() << "Клиент подключён. Всего клиентов:" << activeClients.size();
}

void ChatServer::onClientMessage(const QString& msg, ClientHandler* sender) {
    qDebug() << "От клиента:" << msg;

    // Пример ответа тому же клиенту
    sender->sendMessage("Сервер получил: " + msg);
}

void ChatServer::onClientDisconnected(ClientHandler* sender) {
    // Находим и удаляем поток из списка
    for (int i = 0; i < activeClients.size(); ++i) {
        if (activeClients[i] == sender) {
            activeClients.removeAt(i);
            sender->deleteLater();
            qDebug() << "Клиент отключён. Осталось клиентов:" << activeClients.size();
            break;
        }
    }
}
