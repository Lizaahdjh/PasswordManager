#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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

private:
    Ui::MainWindow *ui;
    void setupTableColumns();
    void setupConnections();
    void addTestData();
    void applyLightTheme();
    void applyDarkTheme();
    void updateStatusBar();
    void filterTable(const QString &searchText, const QString &category);
};

#endif // MAINWINDOW_H