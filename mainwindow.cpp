#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QGuiApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Password Manager");

    setupTableColumns();
    setupConnections();

    applyLightTheme();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applyLightTheme()
{
    QString styleSheet =
        "QMainWindow { background-color: #f0f4f8; }"
        "QMenuBar { background-color: #e3f2fd; color: #1565c0; border-bottom: 1px solid #bbdef5; font-weight: bold; }"
        "QMenuBar::item:selected { background-color: #90caf9; color: #0d47a1; }"
        "QMenu { background-color: white; border: 1px solid #bbdef5; border-radius: 4px; }"
        "QMenu::item:selected { background-color: #bbdef5; color: #0d47a1; }"
        "QToolBar { background-color: #e3f2fd; border-bottom: 1px solid #bbdef5; spacing: 4px; }"
        "QToolButton { color: #1565c0; font-weight: bold; border-radius: 4px; padding: 6px 10px; }"
        "QToolButton:hover { background-color: #90caf9; color: #0d47a1; }"
        "QComboBox { background-color: white; border: 1px solid #90caf9; border-radius: 4px; padding: 5px 8px; color: #1565c0; }"
        "QComboBox:hover { border-color: #42a5f5; }"
        "QLineEdit { background-color: white; border: 1px solid #90caf9; border-radius: 4px; padding: 6px 8px; color: #1565c0; }"
        "QLineEdit:focus { border-color: #42a5f5; }"
        "QPushButton { background-color: #e3f2fd; border: 1px solid #90caf9; border-radius: 4px; padding: 6px 12px; color: #1565c0; font-weight: bold; }"
        "QPushButton:hover { background-color: #90caf9; border-color: #42a5f5; color: #0d47a1; }"
        "QPushButton#clearButton { background-color: #ffebee; border-color: #ef9a9a; color: #c62828; }"
        "QPushButton#clearButton:hover { background-color: #ffcdd2; }"
        "QTableView { background-color: white; alternate-background-color: #f8fbff; gridline-color: #e3f2fd; border: 1px solid #bbdef5; border-radius: 4px; }"
        "QTableView::item:selected { background-color: #90caf9; color: #0d47a1; }"
        "QHeaderView::section { background-color: #e3f2fd; padding: 6px; border: none; border-right: 1px solid #bbdef5; border-bottom: 1px solid #bbdef5; color: #1565c0; font-weight: bold; }"
        "QHeaderView::section:hover { background-color: #bbdef5; }"
        "QStatusBar { background-color: #e3f2fd; color: #1565c0; border-top: 1px solid #bbdef5; }"
        "QLabel { color: #1565c0; font-weight: 500; }"
        "QLabel#lblStatus, QLabel#lblTotal, QLabel#lblFiltered { color: #42a5f5; font-size: 11px; }";

    setStyleSheet(styleSheet);
    if (ui->lblStatus) ui->lblStatus->setText("Light theme applied");
}

void MainWindow::applyDarkTheme()
{
    QString styleSheet =
        "QMainWindow { background-color: #2c2b26; }"
        "QMenuBar { background-color: #3d3b35; color: #f5d742; border-bottom: 1px solid #5a564c; font-weight: bold; }"
        "QMenuBar::item:selected { background-color: #5a564c; color: #ffea4a; }"
        "QMenu { background-color: #3d3b35; border: 1px solid #5a564c; border-radius: 4px; color: #f5d742; }"
        "QMenu::item:selected { background-color: #5a564c; color: #ffea4a; }"
        "QToolBar { background-color: #3d3b35; border-bottom: 1px solid #5a564c; spacing: 4px; }"
        "QToolButton { color: #f5d742; font-weight: bold; border-radius: 4px; padding: 6px 10px; }"
        "QToolButton:hover { background-color: #5a564c; color: #ffea4a; }"
        "QComboBox { background-color: #2c2b26; border: 1px solid #5a564c; border-radius: 4px; padding: 5px 8px; color: #f5d742; }"
        "QComboBox:hover { border-color: #f5d742; }"
        "QLineEdit { background-color: #2c2b26; border: 1px solid #5a564c; border-radius: 4px; padding: 6px 8px; color: #f5d742; }"
        "QLineEdit:focus { border-color: #f5d742; }"
        "QPushButton { background-color: #3d3b35; border: 1px solid #5a564c; border-radius: 4px; padding: 6px 12px; color: #f5d742; font-weight: bold; }"
        "QPushButton:hover { background-color: #5a564c; border-color: #f5d742; color: #ffea4a; }"
        "QPushButton#clearButton { background-color: #4a3a35; border-color: #8b5a4a; color: #ffaa66; }"
        "QPushButton#clearButton:hover { background-color: #5c4840; }"
        "QTableView { background-color: #2c2b26; alternate-background-color: #33312c; gridline-color: #4a4842; border: 1px solid #5a564c; border-radius: 4px; color: #f5d742; }"
        "QTableView::item:selected { background-color: #5a564c; color: #ffea4a; }"
        "QHeaderView::section { background-color: #3d3b35; padding: 6px; border: none; border-right: 1px solid #5a564c; border-bottom: 1px solid #5a564c; color: #f5d742; font-weight: bold; }"
        "QHeaderView::section:hover { background-color: #5a564c; color: #ffea4a; }"
        "QStatusBar { background-color: #3d3b35; color: #f5d742; border-top: 1px solid #5a564c; }"
        "QLabel { color: #f5d742; font-weight: 500; }"
        "QLabel#lblStatus, QLabel#lblTotal, QLabel#lblFiltered { color: #d4c43a; font-size: 11px; }";

    setStyleSheet(styleSheet);
    if (ui->lblStatus) ui->lblStatus->setText("Dark theme applied");
}

void MainWindow::onThemeLightBlue()
{
    applyLightTheme();
}

void MainWindow::onThemeDarkYellow()
{
    applyDarkTheme();
}

void MainWindow::setupTableColumns()
{
    if (!ui->tableViewAccounts) return;

    QStandardItemModel* model = new QStandardItemModel(0, 7, this);
    QStringList headers;
    headers << "ID" << "Title" << "Username" << "Password" << "Website" << "Category" << "Updated At";
    model->setHorizontalHeaderLabels(headers);
    ui->tableViewAccounts->setModel(model);

    ui->tableViewAccounts->setColumnWidth(0, 50);
    ui->tableViewAccounts->setColumnWidth(1, 150);
    ui->tableViewAccounts->setColumnWidth(2, 120);
    ui->tableViewAccounts->setColumnWidth(3, 100);
    ui->tableViewAccounts->setColumnWidth(4, 150);
    ui->tableViewAccounts->setColumnWidth(5, 100);
    ui->tableViewAccounts->setColumnWidth(6, 120);

    ui->tableViewAccounts->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewAccounts->verticalHeader()->setDefaultSectionSize(30);

    ui->tableViewAccounts->setEditTriggers(
        QAbstractItemView::DoubleClicked |
        QAbstractItemView::EditKeyPressed |
        QAbstractItemView::SelectedClicked
        );

    ui->tableViewAccounts->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewAccounts->setSelectionMode(QAbstractItemView::SingleSelection);

    addTestData();

    updateStatusBar();
}

void MainWindow::addTestData()
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableViewAccounts->model());
    if (!model) return;

    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    QList<QStandardItem*> row1;
    row1 << new QStandardItem("1")
         << new QStandardItem("Google Account")
         << new QStandardItem("user@gmail.com")
         << new QStandardItem("pass123456")
         << new QStandardItem("https://google.com")
         << new QStandardItem("Logins")
         << new QStandardItem(currentDate);
    model->appendRow(row1);

    QList<QStandardItem*> row2;
    row2 << new QStandardItem("2")
         << new QStandardItem("GitHub")
         << new QStandardItem("developer@github.com")
         << new QStandardItem("ghp_token_xyz789")
         << new QStandardItem("https://github.com")
         << new QStandardItem("Development")
         << new QStandardItem(currentDate);
    model->appendRow(row2);

    QList<QStandardItem*> row3;
    row3 << new QStandardItem("3")
         << new QStandardItem("Facebook")
         << new QStandardItem("user@facebook.com")
         << new QStandardItem("fb_password_2024")
         << new QStandardItem("https://facebook.com")
         << new QStandardItem("Social Media")
         << new QStandardItem(currentDate);
    model->appendRow(row3);

    QList<QStandardItem*> row4;
    row4 << new QStandardItem("4")
         << new QStandardItem("PrivatBank")
         << new QStandardItem("john.doe")
         << new QStandardItem("bank_pass_2024")
         << new QStandardItem("https://privatbank.ua")
         << new QStandardItem("Banking")
         << new QStandardItem(currentDate);
    model->appendRow(row4);

    QList<QStandardItem*> row5;
    row5 << new QStandardItem("5")
         << new QStandardItem("Netflix")
         << new QStandardItem("user@netflix.com")
         << new QStandardItem("netflix_pass")
         << new QStandardItem("https://netflix.com")
         << new QStandardItem("Entertainment")
         << new QStandardItem(currentDate);
    model->appendRow(row5);
}

void MainWindow::setupConnections()
{
    if (ui->actionNew_Entry) connect(ui->actionNew_Entry, &QAction::triggered, this, &MainWindow::onNewEntry);
    if (ui->actionEdit) connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onEdit);
    if (ui->actionDelete) connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDelete);
    if (ui->actionCopy_Username) connect(ui->actionCopy_Username, &QAction::triggered, this, &MainWindow::onCopyUsername);
    if (ui->actionCopy_Password) connect(ui->actionCopy_Password, &QAction::triggered, this, &MainWindow::onCopyPassword);
    if (ui->actionSave) connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSave);
    if (ui->actionExit) connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExit);
    if (ui->actionGenerate) connect(ui->actionGenerate, &QAction::triggered, this, &MainWindow::onGenerate);
    if (ui->actionAbout) connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    if (ui->actionThemeLightBlue) connect(ui->actionThemeLightBlue, &QAction::triggered, this, &MainWindow::onThemeLightBlue);
    if (ui->actionThemeDarkYellow) connect(ui->actionThemeDarkYellow, &QAction::triggered, this, &MainWindow::onThemeDarkYellow);

    if (ui->searchEdit) connect(ui->searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    if (ui->clearButton) connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearSearch);
    if (ui->comboBox) connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onCategoryChanged);
}

void MainWindow::onNewEntry()
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableViewAccounts->model());
    if (!model) return;

    int newId = model->rowCount() + 1;
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    QList<QStandardItem*> newRow;
    newRow << new QStandardItem(QString::number(newId))
           << new QStandardItem("New Entry")
           << new QStandardItem("")
           << new QStandardItem("")
           << new QStandardItem("")
           << new QStandardItem("")
           << new QStandardItem(currentDate);
    model->appendRow(newRow);

    QModelIndex newIndex = model->index(model->rowCount() - 1, 1);
    ui->tableViewAccounts->setCurrentIndex(newIndex);
    ui->tableViewAccounts->edit(newIndex);

    updateStatusBar();
    if (ui->lblStatus) ui->lblStatus->setText("New entry created");
}

void MainWindow::onEdit()
{
    QModelIndex current = ui->tableViewAccounts->currentIndex();
    if (current.isValid()) {
        ui->tableViewAccounts->edit(current);
        if (ui->lblStatus) ui->lblStatus->setText("Editing entry");
    } else {
        QMessageBox::information(this, "Edit", "Please select an entry to edit");
    }
}

void MainWindow::onDelete()
{
    QModelIndex current = ui->tableViewAccounts->currentIndex();
    if (!current.isValid()) {
        QMessageBox::information(this, "Delete", "Please select an entry to delete");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Delete Record",
        "Are you sure you want to delete the selected record?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableViewAccounts->model());
        if (model) {
            model->removeRow(current.row());
            for (int i = 0; i < model->rowCount(); i++) {
                model->setData(model->index(i, 0), QString::number(i + 1));
            }
            updateStatusBar();
            if (ui->lblStatus) ui->lblStatus->setText("Entry deleted");
        }
    }
}

void MainWindow::onCopyUsername()
{
    QModelIndex current = ui->tableViewAccounts->currentIndex();
    if (current.isValid()) {
        QModelIndex usernameIndex = current.sibling(current.row(), 2);
        QString username = ui->tableViewAccounts->model()->data(usernameIndex).toString();

        if (!username.isEmpty()) {
            QGuiApplication::clipboard()->setText(username);
            if (ui->lblStatus) ui->lblStatus->setText("Username copied: " + username);
        } else {
            if (ui->lblStatus) ui->lblStatus->setText("No username to copy");
        }
    } else {
        QMessageBox::information(this, "Copy", "Please select an entry");
    }
}

void MainWindow::onCopyPassword()
{
    QModelIndex current = ui->tableViewAccounts->currentIndex();
    if (current.isValid()) {
        // Password в колонці 3 (0-ID, 1-Title, 2-Username, 3-Password)
        QModelIndex passwordIndex = current.sibling(current.row(), 3);
        QString password = ui->tableViewAccounts->model()->data(passwordIndex).toString();

        if (!password.isEmpty()) {
            QGuiApplication::clipboard()->setText(password);
            if (ui->lblStatus) ui->lblStatus->setText("Password copied");
        } else {
            if (ui->lblStatus) ui->lblStatus->setText("No password to copy");
        }
    } else {
        QMessageBox::information(this, "Copy", "Please select an entry");
    }
}

void MainWindow::onSave()
{
    QMessageBox::information(this, "Save",
                             "Save functionality will be implemented in future practical works\n"
                             "Data will be saved to SQLite database");
    if (ui->lblStatus) ui->lblStatus->setText("Ready to save");
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onGenerate()
{
    QMessageBox::information(this, "Generate Password",
                             "Password generator will be implemented in future practical works\n"
                             "Will generate secure random passwords");
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Password Manager",
                       "Password Manager v1.0\n\n"
                       "A secure password management application\n"
                       "Built with Qt Framework\n\n"
                       "Features:\n"
                       "• Store and manage passwords\n"
                       "• Copy usernames and passwords\n"
                       "• Search and filter entries\n"
                       "• Light & Dark themes\n"
                       "• In-place cell editing\n\n"
                       "Created for Practical Work No.16\n"
                       "Course: Qt Desktop Development");
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    filterTable(text, ui->comboBox->currentText());
    if (ui->lblStatus) {
        if (text.isEmpty()) {
            ui->lblStatus->setText("Search cleared");
        } else {
            ui->lblStatus->setText("Searching: " + text);
        }
    }
}

void MainWindow::onClearSearch()
{
    if (ui->searchEdit) ui->searchEdit->clear();
    filterTable("", ui->comboBox->currentText());
    if (ui->lblStatus) ui->lblStatus->setText("Search cleared");
}

void MainWindow::onCategoryChanged(int index)
{
    QString category = ui->comboBox->currentText();
    filterTable(ui->searchEdit->text(), category);
    if (ui->lblStatus) ui->lblStatus->setText("Filtering by category: " + category);
}

void MainWindow::filterTable(const QString &searchText, const QString &category)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableViewAccounts->model());
    if (!model) return;

    int filteredCount = 0;

    for (int i = 0; i < model->rowCount(); i++) {
        bool showRow = true;

        if (category != "All") {
            QString itemCategory = model->data(model->index(i, 5)).toString(); // Category в колонці 5
            if (itemCategory != category) {
                showRow = false;
            }
        }

        if (showRow && !searchText.isEmpty()) {
            QString title = model->data(model->index(i, 1)).toString();
            QString username = model->data(model->index(i, 2)).toString();
            QString website = model->data(model->index(i, 4)).toString();

            if (!title.contains(searchText, Qt::CaseInsensitive) &&
                !username.contains(searchText, Qt::CaseInsensitive) &&
                !website.contains(searchText, Qt::CaseInsensitive)) {
                showRow = false;
            }
        }

        ui->tableViewAccounts->setRowHidden(i, !showRow);
        if (showRow) filteredCount++;
    }

    if (ui->lblFiltered) {
        ui->lblFiltered->setText(QString("Filtered: %1").arg(filteredCount));
    }
}

void MainWindow::updateStatusBar()
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableViewAccounts->model());
    if (model && ui->lblTotal && ui->lblFiltered) {
        int total = model->rowCount();
        ui->lblTotal->setText(QString("Total: %1").arg(total));
        ui->lblFiltered->setText(QString("Filtered: %1").arg(total));
    }
}