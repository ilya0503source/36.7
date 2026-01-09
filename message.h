#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>

class Message{
private:
    QString m_sender;
    QString m_recipient;
    QString m_text;
public:
    Message(const QString& sender, const QString& recipient, const QString& text):
        m_sender(sender),m_recipient(recipient),m_text(text){}
    QString getSender() const {return m_sender;}
    QString getRecipient() const {return m_recipient;}
    QString getText() const {return m_text;}
    void setSender(const QString& sender){m_sender = sender;}
    void setRecipient(const QString& recipient){m_recipient = recipient;}
    void setText(const QString& text){m_text = text;}
};

#endif // MESSAGE_H
