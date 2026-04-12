#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
    : m_isOpen(false)
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::open(const QString &filePath)
{
    QDir dir;
    QString dbDir = QFileInfo(filePath).absolutePath();
    if (!dir.exists(dbDir)) {
        dir.mkpath(dbDir);
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(filePath);

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qDebug() << "Database open error:" << m_lastError;
        m_isOpen = false;
        return false;
    }

    m_isOpen = true;

    if (!initializeSchema()) {
        qDebug() << "Schema initialization error:" << m_lastError;
        return false;
    }

    qDebug() << "Database opened successfully:" << filePath;
    return true;
}

void DatabaseManager::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_isOpen = false;
}

bool DatabaseManager::isOpen() const
{
    return m_isOpen && m_db.isOpen();
}

QSqlDatabase DatabaseManager::database() const
{
    return m_db;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::initializeSchema()
{
    QSqlQuery query(m_db);

    QString createTableSQL =
        "CREATE TABLE IF NOT EXISTS password_items ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL,"
        "website TEXT,"
        "category TEXT,"
        "updated_at TEXT NOT NULL"
        ")";

    if (!query.exec(createTableSQL)) {
        m_lastError = query.lastError().text();
        qDebug() << "Create table error:" << m_lastError;
        return false;
    }

    qDebug() << "Schema initialized successfully";
    return true;
}