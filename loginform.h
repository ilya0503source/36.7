#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "client.h"

namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget* parent = nullptr);
    void setClient(Client* client);
    ~LoginForm();

signals:
    void registrationRequested();
    void loginSuccessful(const QString& login);
    void loginResponse(const QString& response);

private slots:
    void on_registrationPushButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

public slots:
    void onLoginResponse(const QString& response);

private:
    Ui::LoginForm* ui;
    Client* m_client;
};

#endif // LOGINFORM_H
