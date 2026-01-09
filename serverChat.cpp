#include "serverChat.h"
#include "chatDB.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger.h"


ClientHandler::ClientHandler(QTcpSocket* socket,
                             QVector<User>& users,
                             QVector<Message>& messages,
                             QMutex& usersMutex,
                             QMutex& messagesMutex,
                             ServerChat* server,
                             QObject* parent)
    : QThread(parent),
    m_socket(socket),
    m_users(users),
    m_messages(messages),
    m_usersMutex(usersMutex),
    m_messagesMutex(messagesMutex),
    m_server(server)
{
    m_socket->setParent(this);
}


    void ClientHandler::run()
    {
        if (m_socket->socketDescriptor() == -1) {
            LOG("Неверный дескриптор сокета, прерывание");
            emit clientDisconnected();
            return;
        }

        if (m_socket->socketDescriptor() == -1) {
            LOG("Неверный дескриптор сокета, прерывание");
            emit clientDisconnected();
            return;
        }
        LOG("Клиент успешно подключился, сокет:" + m_socket->socketDescriptor());

        connect(m_socket, &QTcpSocket::readyRead, this, [this]() {
            QByteArray data = m_socket->readAll();
            QList<QByteArray> parts = data.split(';');

            if (parts.at(0) == "registration") {
                QString login = parts.at(1);
                m_login = login;
                QString password = parts.at(2);
                QMutexLocker usersLocker(&m_usersMutex);
                auto it = std::find_if(m_users.begin(), m_users.end(),
                                       [&login](const User& user) {
                                           return user.getLogin() == login;
                                       });

                if (it != m_users.end()) {
                    LOG("Ошибка регистрации пользователя, логин занят: " + it->getLogin());
                    QByteArray response = "registration;registration failed: user exists";
                    m_socket->write(response);
                    m_socket->flush();
                }
                else {
                    {
                    m_users.append(User(login, password, false));
                    }
                    {
                    QMutexLocker dbLocker(&m_server->getDbMutex());
                    if (!ChatDB::addUserToDB(User(login, password, false))) {
                        LOG("Не удалось сохранить пользователя в базе данных");
                    }
                }
                    LOG("Новый пользователь " + login + " успешно зарегистировался");
                    QByteArray response = "registration;registration is successful";
                    m_socket->write(response);
                    m_socket->flush();
                    emit userListChanged();
                }
            }
            else if (parts.at(0) == "login") {
                QString login = parts.at(1);
                m_login = login;
                QString password = parts.at(2);


                User* foundUser = nullptr;
                {
                    QMutexLocker usersLocker(&m_usersMutex);
                    auto it = std::find_if(m_users.begin(), m_users.end(),
                                           [&login](const User& user) {
                                               return user.getLogin() == login;
                                           });
                    if (it != m_users.end()) {
                        foundUser = &(*it);
                    }
                }

                if (!foundUser) {
                    LOG("Ошибка авторизации: некорректный логин");
                    QByteArray response = "login;login is incorrect";
                    m_socket->write(response);
                    m_socket->flush();
                } else if (foundUser->getPassword() != password) {
                    LOG("Ошибка авторизации: некорректный пароль");
                    QByteArray response = "login;password is incorrect";
                    m_socket->write(response);
                    m_socket->flush();
                } else if (foundUser->getBanStatus()) {
                    LOG("Ошибка авторизации: пользователь забанен");
                    QByteArray response = "login;user is banned";
                    m_socket->write(response);
                    m_socket->flush();
                } else {
                    LOG("Пользователь " + login + " успешно авторизовался");
                    QByteArray response = "login;login is successful";
                    m_socket->write(response);
                    m_socket->flush();
                }
            }
            else if (parts.at(0) == "sendmessage") {
                QString sender = parts.at(1);
                QString recipient = parts.at(2);
                QString text = parts.at(3);

                Message newMsg(sender, recipient, text);
                {
                    QMutexLocker messagesLocker(&m_messagesMutex);
                    m_messages.append(newMsg);
                }

                {
                    QMutexLocker dbLocker(&m_server->getDbMutex());
                    if (!ChatDB::addMessageToDB(newMsg)) {
                        LOG("Не удалось сохранить сообщение в базе данных");
                    }
                }
                emit messageReceived(newMsg);
            }
            else if (parts.at(0) == "showusers") {
                QString result = "showusers;";
                for (const auto& user : m_users) {
                    result += user.getLogin() + ";";
                }
                QByteArray response = result.toUtf8();
                m_socket->write(response);
                m_socket->flush();
            }
            else if (parts.at(0) == "showmessages") {
            sendMessagesToClient();
            }
            else {
                LOG("Была запрошена некорректная команда пользователем");
            }
        });


        connect(m_socket, &QTcpSocket::disconnected, this, [this]() {
            LOG("Клиент отключился, сокет: " + m_socket->socketDescriptor());
            m_socket->close();
            emit clientDisconnected();
        });
        exec();
        qDebug() << "ClientHandler thread finished for socket:" << m_socket->socketDescriptor();
    }

    ServerChat::ServerChat(QObject* parent)
        : QTcpServer(parent)
    {
        m_users.clear();
        m_messages.clear();

        if (!ChatDB::loadUsers(m_users)) {
            LOG("Не удалось загрузить пользователей из базы данных");
        }
        if (!ChatDB::loadMessages(m_messages)) {
            LOG("Не удалось загрузить сообщения из базы данных");
        }
    }

    bool ServerChat::startServer(quint16 port)
    {
        if (!listen(QHostAddress::Any, port)) {
            LOG("Не удалось запустить сервер: " + errorString());
            return false;
        }
        LOG("Сервер запущен по порту: " + port);
        return true;
    }

    void ServerChat::incomingConnection(qintptr socketDescriptor)
    {
        LOG("Новое подключение клиента, дескриптор сокета: " + socketDescriptor);

        QTcpSocket* socket = new QTcpSocket();
        socket->setSocketDescriptor(socketDescriptor);

        ClientHandler* handler = new ClientHandler(
            socket, m_users, m_messages, m_usersMutex, m_messagesMutex, this, this);
        m_clientHandlers.append(handler);

        connect(handler, &ClientHandler::messageReceived,
                this, &ServerChat::broadcastNewMessage);
        connect(handler, &ClientHandler::userListChanged, this, &ServerChat::userListChanged);
        connect(handler, &ClientHandler::clientDisconnected, this, [this, handler]() {
            handler->quit();
            handler->wait(1000);
            handler->deleteLater();
            m_clientHandlers.removeAll(handler);
                emit userListChanged();
        });
        handler->start();
    }
    void ServerChat::disconnectUser(const QString& login)
    {
        for (ClientHandler* handler : m_clientHandlers) {
            if (handler->getLogin() == login) {
                handler->disconnectClient();
                LOG("Пользователь " + login + " отключен");
                emit userListChanged();
                return;
            }
        }
        LOG("Пользователь" + login + "не найден для отключения");
    }

    void ClientHandler::sendMessagesToClient()
    {
        QString result = "showmessages;";
        bool firstMessage = true;

        for (const auto& message : m_messages) {
            if (message.getRecipient() == "all" ||
                message.getRecipient() == m_login ||
                message.getSender() == m_login) {

                if (!firstMessage) {
                    result += "|";
                }
                result += message.getSender() + ";" +
                          message.getRecipient() + ";" +
                          message.getText();
                firstMessage = false;
            }
        }

        QByteArray response = result.toUtf8();
        m_socket->write(response);
        m_socket->flush();
    }


    void ClientHandler::sendSingleMessageToClient(const Message& msg)
    {
        if (msg.getRecipient() == "all" ||
            msg.getRecipient() == m_login ||
            msg.getSender() == m_login) {

            QString result = "showmessages;" +
                             msg.getSender() + ";" +
                             msg.getRecipient() + ";" +
                             msg.getText();

            QByteArray response = result.toUtf8();
            m_socket->write(response);
            m_socket->flush();
        }
    }

    void ClientHandler::disconnectClient()
    {
        if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
            LOG("Отправка команды отключения пользователю: " + m_login);
            QByteArray notification = "disconnectuser;";
            m_socket->write(notification);
            m_socket->flush();
            m_socket->disconnectFromHost();
            m_socket->waitForDisconnected(1000);
        }
    }

    void ServerChat::broadcastNewMessage(const Message& msg)
    {
    emit messageReceived(msg);
        for (ClientHandler* handler : m_clientHandlers) {
            if (handler->isRunning()) {
                handler->sendSingleMessageToClient(msg);
            }
        }
    }

    QVector<ClientHandler*> ServerChat::getActiveClients() {
        LOG("Поиск активных клиентов. Активных подключений: " + m_clientHandlers.size());
        return m_clientHandlers;
    }
