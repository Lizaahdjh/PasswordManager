#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PasswordEntry.h"
#include "PasswordTableModel.h"

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
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    Ui::MainWindow *ui;
    PasswordTableModel *m_tableModel;
    PasswordRepository *m_repository;

    void setupTableColumns();
    void setupConnections();
    void applyLightTheme();
    void applyDarkTheme();
    void updateStatusBar();
    void filterTable(const QString &searchText, const QString &category);
    void loadDataFromDatabase();
    void addTestDataToDatabase();
    void saveEntryToDatabase(const PasswordEntry &entry);
    void updateEntryInDatabase(const PasswordEntry &entry);
    void deleteEntryFromDatabase(int id);
    void showErrorMessage(const QString &title, const QString &message);
};

#endif // MAINWINDOW_H