#ifndef PASSWORDREPOSITORY_H
#define PASSWORDREPOSITORY_H

#include <QList>
#include <QString>
#include <QObject>
#include <QSqlDatabase>
#include "PasswordEntry.h"

class PasswordRepository : public QObject
{
    Q_OBJECT

public:
    explicit PasswordRepository(const QSqlDatabase &database, QObject *parent = nullptr);

    QList<PasswordEntry> loadAll() const;
    bool insert(const PasswordEntry &entry);
    bool update(const PasswordEntry &entry);
    bool remove(int id);
    PasswordEntry findById(int id) const;
    int getLastInsertId() const;

private:
    QSqlDatabase m_db;
};

#endif // PASSWORDREPOSITORY_H