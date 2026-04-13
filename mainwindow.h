#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PasswordEntry.h"
#include "PasswordTableModel.h"
#include "PasswordFilterProxyModel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class PasswordRepository;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewEntry();
    void onEdit();
    void onDelete();
    void onCopyUsername();
    void onCopyPassword();
    void onSave();
    void onExit();
    void onGenerate();
    void onAbout();
    void onThemeLightBlue();
    void onThemeDarkYellow();
    void onSearchTextChanged(const QString &text);
    void onClearSearch();
    void onCategoryChanged(int index);
    void onResetFilters();

private:
    Ui::MainWindow *ui;
    PasswordTableModel *m_tableModel;
    PasswordFilterProxyModel *m_proxyModel;
    PasswordRepository *m_repository;

    void setupTableColumns();
    void setupConnections();
    void applyLightTheme();
    void applyDarkTheme();
    void updateStatusBar();
    void updateEmptyState();
    void loadDataFromDatabase();
    void addTestDataToDatabase();
    void reloadTable();
    void showErrorMessage(const QString &title, const QString &message);

    int getCurrentSourceRow() const;
    PasswordEntry getCurrentEntry() const;
};

#endif // MAINWINDOW_H