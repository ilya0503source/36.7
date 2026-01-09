#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>
#include "client.h"

class LoginForm;
class RegistrationForm;

namespace Ui {
class StartScreen;
}

class StartScreen : public QDialog
{
    Q_OBJECT

public:
    explicit StartScreen(QWidget* parent, Client* client);
    ~StartScreen();

private slots:
    void onRegistrationRequested();
    void onLoginRequested();

private:
    Ui::StartScreen* ui;
    LoginForm* m_loginForm;
    RegistrationForm* m_regForm;
    Client* m_client;
};

#endif // STARTSCREEN_H
