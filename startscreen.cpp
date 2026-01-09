#include "startscreen.h"
#include "ui_startscreen.h"
#include "loginform.h"
#include "registrationform.h"
#include <QDebug>

StartScreen::~StartScreen()
{
    delete ui;
}

void StartScreen::onRegistrationRequested()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void StartScreen::onLoginRequested()
{
    ui->stackedWidget->setCurrentIndex(0);
}

StartScreen::StartScreen(QWidget* parent, Client* client)
    : QDialog(parent), m_client(client)
{
    ui = new Ui::StartScreen;
    ui->setupUi(this);

    m_loginForm = qobject_cast<LoginForm*>(ui->stackedWidget->widget(0));
    m_regForm   = qobject_cast<RegistrationForm*>(ui->stackedWidget->widget(1));

    if (!m_loginForm || !m_regForm) {
        return;
    }

    if (m_client) {
        m_loginForm->setClient(m_client);
        m_regForm->setClient(m_client);
    }

    m_loginForm->setClient(m_client);
    connect(m_client, &Client::loginResponse,
            m_loginForm, &LoginForm::onLoginResponse);
    m_regForm->setClient(m_client);
    connect(m_client, &Client::registrationResponse,
            m_regForm, &RegistrationForm::onRegistrationResponse);
    connect(m_loginForm, &LoginForm::registrationRequested,
            this, &StartScreen::onRegistrationRequested);
    connect(m_regForm, &RegistrationForm::loginRequested,
            this, &StartScreen::onLoginRequested);
    connect(m_loginForm, &LoginForm::loginSuccessful,
            this, &StartScreen::close);
    connect(m_regForm, &RegistrationForm::registrationSuccessful,
            this, &StartScreen::close);

    this->setAttribute(Qt::WA_DeleteOnClose, false);
}
