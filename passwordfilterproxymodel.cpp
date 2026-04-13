#include "PasswordFilterProxyModel.h"
#include <QModelIndex>

PasswordFilterProxyModel::PasswordFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_searchText("")
    , m_categoryFilter("")
{
}

void PasswordFilterProxyModel::setSearchText(const QString &text)
{
    m_searchText = text;
    refreshFilter();
}

void PasswordFilterProxyModel::setCategoryFilter(const QString &category)
{
    m_categoryFilter = category;
    refreshFilter();
}

void PasswordFilterProxyModel::clearFilters()
{
    m_searchText.clear();
    m_categoryFilter.clear();
    refreshFilter();
}

void PasswordFilterProxyModel::refreshFilter()
{
    invalidateFilter();
}

bool PasswordFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex titleIndex = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex categoryIndex = sourceModel()->index(sourceRow, 5, sourceParent);

    QString title = sourceModel()->data(titleIndex).toString();
    QString category = sourceModel()->data(categoryIndex).toString();

    bool textMatches = m_searchText.isEmpty() ||
                       title.contains(m_searchText, Qt::CaseInsensitive);

    bool categoryMatches = m_categoryFilter.isEmpty() ||
                           m_categoryFilter == "All" ||
                           category == m_categoryFilter;

    return textMatches && categoryMatches;
}