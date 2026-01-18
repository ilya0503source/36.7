#include "registrationform.h"
#include "ui_registrationform.h"
#include <QDebug>
#include "mainwindow.h"
#include <QMessageBox>

RegistrationForm::RegistrationForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationForm)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Войти");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Отмена");
}

RegistrationForm::~RegistrationForm()
{
    delete ui;
}

void RegistrationForm::on_loginButton_clicked()
{
    emit loginRequested();
}

void RegistrationForm::setClient(Client* client) {
    m_client = client;
    connect(m_client, &Client::messageReceived,
            this, &RegistrationForm::onRegistrationResponse);
}
void RegistrationForm::on_buttonBox_accepted() {
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();
    QString confirmPassword = ui->passwordConfirmEdit->text();

    if (login.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }
    if (password != confirmPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают!");
        return;
    }

    QString hashStr = m_client->hashPassword(password);
    QString message = "registration;" + login + ";" + hashStr;
    m_client->sendMessage(message);
}

void RegistrationForm::onRegistrationResponse(const QString& response) {
    if (response == "registration is successful") {
        QMessageBox::information(this, "Регистрация", "Вы успешно зарегистрировались!");
        QString login = ui->loginEdit->text();
        MainWindow* mainWindow = new MainWindow(nullptr, m_client,login);
        mainWindow->show();

        emit registrationSuccessful(login);
        this->close();
    } else {
        QMessageBox::critical(this, "Ошибка", "Регистрация не удалась: " + response);
    }
}

void RegistrationForm::on_buttonBox_rejected()
{
    QApplication::quit();
}

