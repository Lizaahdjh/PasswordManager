#include "PasswordRepository.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PasswordRepository::PasswordRepository(const QSqlDatabase &database, QObject *parent)
    : QObject(parent)
    , m_db(database)
{
}

QList<PasswordEntry> PasswordRepository::loadAll() const
{
    QList<PasswordEntry> entries;

    if (!m_db.isOpen()) {
        qDebug() << "Database is not open";
        return entries;
    }

    QSqlQuery query(m_db);
    QString selectSQL =
        "SELECT id, title, username, password, website, category, updated_at "
        "FROM password_items ORDER BY id";

    if (!query.exec(selectSQL)) {
        qDebug() << "Load all error:" << query.lastError().text();
        return entries;
    }

    while (query.next()) {
        PasswordEntry entry;
        entry.id = query.value(0).toInt();
        entry.title = query.value(1).toString();
        entry.username = query.value(2).toString();
        entry.password = query.value(3).toString();
        entry.website = query.value(4).toString();
        entry.category = query.value(5).toString();
        entry.updatedAt = query.value(6).toString();
        entries.append(entry);
    }

    qDebug() << "Loaded" << entries.size() << "entries from database";
    return entries;
}

bool PasswordRepository::insert(const PasswordEntry &entry)
{
    if (!m_db.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }

    QSqlQuery query(m_db);
    QString insertSQL =
        "INSERT INTO password_items (title, username, password, website, category, updated_at) "
        "VALUES (:title, :username, :password, :website, :category, :updatedAt)";

    query.prepare(insertSQL);
    query.bindValue(":title", entry.title);
    query.bindValue(":username", entry.username);
    query.bindValue(":password", entry.password);
    query.bindValue(":website", entry.website);
    query.bindValue(":category", entry.category);
    query.bindValue(":updatedAt", entry.updatedAt);

    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        return false;
    }

    qDebug() << "Inserted entry:" << entry.title;
    return true;
}

bool PasswordRepository::update(const PasswordEntry &entry)
{
    if (!m_db.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }

    QSqlQuery query(m_db);
    QString updateSQL =
        "UPDATE password_items SET "
        "title = :title, username = :username, password = :password, "
        "website = :website, category = :category, updated_at = :updatedAt "
        "WHERE id = :id";

    query.prepare(updateSQL);
    query.bindValue(":title", entry.title);
    query.bindValue(":username", entry.username);
    query.bindValue(":password", entry.password);
    query.bindValue(":website", entry.website);
    query.bindValue(":category", entry.category);
    query.bindValue(":updatedAt", entry.updatedAt);
    query.bindValue(":id", entry.id);

    if (!query.exec()) {
        qDebug() << "Update error:" << query.lastError().text();
        return false;
    }

    qDebug() << "Updated entry ID:" << entry.id;
    return true;
}

bool PasswordRepository::remove(int id)
{
    if (!m_db.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }

    QSqlQuery query(m_db);
    QString deleteSQL = "DELETE FROM password_items WHERE id = :id";

    query.prepare(deleteSQL);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Delete error:" << query.lastError().text();
        return false;
    }

    qDebug() << "Deleted entry ID:" << id;
    return true;
}

PasswordEntry PasswordRepository::findById(int id) const
{
    PasswordEntry entry;

    if (!m_db.isOpen()) {
        return entry;
    }

    QSqlQuery query(m_db);
    QString selectSQL =
        "SELECT id, title, username, password, website, category, updated_at "
        "FROM password_items WHERE id = :id";

    query.prepare(selectSQL);
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        entry.id = query.value(0).toInt();
        entry.title = query.value(1).toString();
        entry.username = query.value(2).toString();
        entry.password = query.value(3).toString();
        entry.website = query.value(4).toString();
        entry.category = query.value(5).toString();
        entry.updatedAt = query.value(6).toString();
    }

    return entry;
}

int PasswordRepository::getLastInsertId() const
{
    QSqlQuery query(m_db);
    if (query.exec("SELECT last_insert_rowid()") && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}