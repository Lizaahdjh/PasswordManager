#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool open(const QString &filePath);
    void close();
    bool isOpen() const;
    QSqlDatabase database() const;
    QString lastError() const;

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool initializeSchema();

    QSqlDatabase m_db;
    bool m_isOpen;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H