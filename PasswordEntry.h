#ifndef PASSWORDENTRY_H
#define PASSWORDENTRY_H

#include <QString>
#include <QDateTime>

struct PasswordEntry
{
    int id = 0;
    QString title;
    QString username;
    QString password;
    QString website;
    QString category;
    QString updatedAt;

    PasswordEntry() = default;

    PasswordEntry(int id, const QString &title, const QString &username,
                  const QString &password, const QString &website,
                  const QString &category, const QString &updatedAt)
        : id(id)
        , title(title)
        , username(username)
        , password(password)
        , website(website)
        , category(category)
        , updatedAt(updatedAt)
    {
    }

    static QString getCurrentTimestamp() {
        return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
};

#endif // PASSWORDENTRY_H