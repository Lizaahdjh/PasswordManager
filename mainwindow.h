#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <QThread>
#include "PasswordEntry.h"
#include "PasswordTableModel.h"
#include "PasswordFilterProxyModel.h"
#include "PasswordLeakChecker.h"
#include "BatchCheckWorker.h"

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
    void onToggleTestMode();
    void onCheckPassword();
    void onCheckAllPasswords();
    void onCancelBatchCheck();
    void onLeakCheckStarted();
    void onLeakCheckCompleted(bool isLeaked, int breachCount);
    void onLeakCheckFailed(const QString &errorMessage);

    void onBatchProgressChanged(int current, int total);
    void onEntryChecked(int entryId, bool isCompromised, int breachCount);
    void onBatchFinished(const BatchCheckResult &result);
    void onBatchError(const QString &errorMessage);

private:
    Ui::MainWindow *ui;
    PasswordTableModel *m_tableModel;
    PasswordFilterProxyModel *m_proxyModel;
    PasswordRepository *m_repository;
    PasswordLeakChecker *m_leakChecker;

    QThread *m_batchThread;
    BatchCheckWorker *m_batchWorker;
    bool m_batchRunning;

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
    void updateEntryInTable(int entryId, bool isCompromised, int breachCount);
    void setBatchCheckEnabled(bool enabled);

    int getCurrentSourceRow() const;
    PasswordEntry getCurrentEntry() const;
};

#endif // MAINWINDOW_H