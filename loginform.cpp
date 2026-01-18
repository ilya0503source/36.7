#include "loginform.h"
#include "ui_loginform.h"
#include <QDebug>
#include "mainwindow.h"
#include <QMessageBox>


LoginForm::LoginForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LoginForm)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Войти");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Отмена");
}

void LoginForm::setClient(Client* client) {
    m_client = client;
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::on_registrationPushButton_clicked()
{
    emit registrationRequested();
}

void LoginForm::on_buttonBox_accepted() {
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }

    QString hashStr = m_client->hashPassword(password);
    QString message = "login;" + login + ";" + hashStr;
    m_client->sendMessage(message);
}

void LoginForm::onLoginResponse(const QString& response) {
    if (response == "login is successful") {
        QString login = ui->loginEdit->text();
        MainWindow* mainWindow = new MainWindow(nullptr, m_client,login);
        mainWindow->show();
        emit loginSuccessful(login);
        this->close();
    } else {
        QMessageBox::critical(this, "Ошибка", "Вход не удался: " + response);
    }
}

void LoginForm::on_buttonBox_rejected()
{
    QApplication::quit();
}

