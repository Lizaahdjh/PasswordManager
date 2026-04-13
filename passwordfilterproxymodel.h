#ifndef PASSWORDFILTERPROXYMODEL_H
#define PASSWORDFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>

class PasswordFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit PasswordFilterProxyModel(QObject *parent = nullptr);

    void setSearchText(const QString &text);
    void setCategoryFilter(const QString &category);
    void clearFilters();
    void refreshFilter();

    QString getSearchText() const { return m_searchText; }
    QString getCategoryFilter() const { return m_categoryFilter; }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_searchText;
    QString m_categoryFilter;
};

#endif // PASSWORDFILTERPROXYMODEL_H