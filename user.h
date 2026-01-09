#ifndef USER_H
#define USER_H

#include <QString>

class User {
private:
    QString m_login;
    QString m_password;
    bool m_banStatus;

public:
    User() : m_login(""), m_password(""), m_banStatus(false) {}
    User(const QString& login, const QString& password, bool banStatus)
        : m_login(login), m_password(password), m_banStatus(banStatus) {}
    QString getLogin() const { return m_login; }
    QString getPassword() const { return m_password; }
    bool getBanStatus() const { return m_banStatus; }
    void setLogin(const QString& login) { m_login = login; }
    void setPassword(const QString& password) { m_password = password; }
    void setBanStatus(bool banStatus) { m_banStatus = banStatus; }
};

#endif // USER_H
