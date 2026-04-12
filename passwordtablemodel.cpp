#include "PasswordTableModel.h"
#include <QDebug>

PasswordTableModel::PasswordTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "ID" << "Title" << "Username" << "Password" << "Website" << "Category" << "Updated At";
}

int PasswordTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_entries.size();
}

int PasswordTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

QVariant PasswordTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_entries.size())
        return QVariant();

    const PasswordEntry &entry = m_entries[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0: return entry.id;
        case 1: return entry.title;
        case 2: return entry.username;
        case 3: return entry.password;
        case 4: return entry.website;
        case 5: return entry.category;
        case 6: return entry.updatedAt;
        default: return QVariant();
        }
    }

    return QVariant();
}

bool PasswordTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole || index.row() >= m_entries.size())
        return false;

    PasswordEntry &entry = m_entries[index.row()];

    switch (index.column()) {
    case 1: entry.title = value.toString(); break;
    case 2: entry.username = value.toString(); break;
    case 3: entry.password = value.toString(); break;
    case 4: entry.website = value.toString(); break;
    case 5: entry.category = value.toString(); break;
    default: return false;
    }

    entry.updatedAt = PasswordEntry::getCurrentTimestamp();

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags PasswordTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 0 || index.column() == 6)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant PasswordTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal && section >= 0 && section < m_headers.size())
        return m_headers[section];

    if (orientation == Qt::Vertical)
        return QString::number(section + 1);

    return QVariant();
}

void PasswordTableModel::setEntries(const QList<PasswordEntry> &entries)
{
    beginResetModel();
    m_entries = entries;
    endResetModel();
}

PasswordEntry PasswordTableModel::getEntryAt(int row) const
{
    if (row >= 0 && row < m_entries.size())
        return m_entries[row];
    return PasswordEntry();
}

void PasswordTableModel::addEntry(const PasswordEntry &entry)
{
    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
    m_entries.append(entry);
    endInsertRows();
}

void PasswordTableModel::updateEntry(int row, const PasswordEntry &entry)
{
    if (row >= 0 && row < m_entries.size()) {
        m_entries[row] = entry;
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }
}

void PasswordTableModel::removeEntry(int row)
{
    if (row >= 0 && row < m_entries.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_entries.removeAt(row);
        endRemoveRows();
    }
}