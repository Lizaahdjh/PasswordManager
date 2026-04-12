#ifndef PASSWORDTABLEMODEL_H
#define PASSWORDTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "PasswordEntry.h"

class PasswordTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PasswordTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setEntries(const QList<PasswordEntry> &entries);
    QList<PasswordEntry> getEntries() const { return m_entries; }
    PasswordEntry getEntryAt(int row) const;
    void addEntry(const PasswordEntry &entry);
    void updateEntry(int row, const PasswordEntry &entry);
    void removeEntry(int row);

private:
    QList<PasswordEntry> m_entries;
    QStringList m_headers;
};

#endif // PASSWORDTABLEMODEL_H