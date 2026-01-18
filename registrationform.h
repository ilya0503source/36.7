#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QWidget>
#include "client.h"

namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationForm(QWidget* parent = nullptr);
    void setClient(Client* client);
    ~RegistrationForm();

signals:
    void loginRequested();
    void registrationSuccessful(const QString& login);


private slots:
    void on_loginButton_clicked();

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

public slots:
    void onRegistrationResponse(const QString& response);

private:
    Ui::RegistrationForm* ui;
    Client* m_client;
};

#endif // REGISTRATIONFORM_H
