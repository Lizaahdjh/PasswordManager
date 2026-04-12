#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DatabaseManager.h"
#include "PasswordRepository.h"
#include <QMessageBox>
#include <QGuiApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_tableModel(nullptr)
    , m_repository(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Password Manager");

    DatabaseManager& dbManager = DatabaseManager::instance();
    QString dbPath = QCoreApplication::applicationDirPath() + "/password_manager.db";

    if (!dbManager.open(dbPath)) {
        showErrorMessage("Database Error",
                         "Failed to open database:\n" + dbManager.lastError() +
                             "\n\nThe application will run in offline mode.");
    }

    m_repository = new PasswordRepository(dbManager.database(), this);

    m_tableModel = new PasswordTableModel(this);

    connect(m_tableModel, &PasswordTableModel::dataChanged,
            this, &MainWindow::onDataChanged);

    ui->tableViewAccounts->setModel(m_tableModel);
    setupTableColumns();
    setupConnections();

    loadDataFromDatabase();

    if (m_tableModel->rowCount() == 0) {
        addTestDataToDatabase();
        loadDataFromDatabase();
    }

    applyLightTheme();
}

MainWindow::~MainWindow()
{
    DatabaseManager::instance().close();
    delete ui;
}

void MainWindow::addTestDataToDatabase()
{
    if (!m_repository || !DatabaseManager::instance().isOpen()) {
        qDebug() << "Cannot add test data: database not available";
        return;
    }

    QString currentDate = PasswordEntry::getCurrentTimestamp();

    PasswordEntry entry1;
    entry1.title = "Google Account";
    entry1.username = "user@gmail.com";
    entry1.password = "google_pass_2024";
    entry1.website = "https://google.com";
    entry1.category = "Logins";
    entry1.updatedAt = currentDate;
    m_repository->insert(entry1);

    PasswordEntry entry2;
    entry2.title = "GitHub";
    entry2.username = "developer@github.com";
    entry2.password = "ghp_token_abc123xyz";
    entry2.website = "https://github.com";
    entry2.category = "Development";
    entry2.updatedAt = currentDate;
    m_repository->insert(entry2);

    PasswordEntry entry3;
    entry3.title = "Facebook";
    entry3.username = "user@facebook.com";
    entry3.password = "fb_password_2024";
    entry3.website = "https://facebook.com";
    entry3.category = "Social Media";
    entry3.updatedAt = currentDate;
    m_repository->insert(entry3);

    PasswordEntry entry4;
    entry4.title = "Privat24";
    entry4.username = "+380501234567";
    entry4.password = "bank_pass_123";
    entry4.website = "https://privat24.ua";
    entry4.category = "Banking";
    entry4.updatedAt = currentDate;
    m_repository->insert(entry4);

    PasswordEntry entry5;
    entry5.title = "Netflix";
    entry5.username = "user@netflix.com";
    entry5.password = "netflix_2024";
    entry5.website = "https://netflix.com";
    entry5.category = "Entertainment";
    entry5.updatedAt = currentDate;
    m_repository->insert(entry5);

    PasswordEntry entry6;
    entry6.title = "Amazon";
    entry6.username = "buyer@amazon.com";
    entry6.password = "amazon_pass_2024";
    entry6.website = "https://amazon.com";
    entry6.category = "Shopping";
    entry6.updatedAt = currentDate;
    m_repository->insert(entry6);

    PasswordEntry entry7;
    entry7.title = "Twitter (X)";
    entry7.username = "user@twitter.com";
    entry7.password = "twitter_pass_2024";
    entry7.website = "https://twitter.com";
    entry7.category = "Social Media";
    entry7.updatedAt = currentDate;
    m_repository->insert(entry7);

    PasswordEntry entry8;
    entry8.title = "LinkedIn";
    entry8.username = "professional@linkedin.com";
    entry8.password = "linkedin_2024";
    entry8.website = "https://linkedin.com";
    entry8.category = "Professional";
    entry8.updatedAt = currentDate;
    m_repository->insert(entry8);

    PasswordEntry entry9;
    entry9.title = "Home Wi-Fi";
    entry9.username = "Admin";
    entry9.password = "wifi_password_123";
    entry9.website = "192.168.1.1";
    entry9.category = "WiFi";
    entry9.updatedAt = currentDate;
    m_repository->insert(entry9);

    PasswordEntry entry10;
    entry10.title = "Windows 11 License";
    entry10.username = "user@microsoft.com";
    entry10.password = "WIN-XXXXX-XXXXX-XXXXX";
    entry10.website = "https://microsoft.com";
    entry10.category = "Software Licenses";
    entry10.updatedAt = currentDate;
    m_repository->insert(entry10);

    qDebug() << "Added 10 test entries to database";
    ui->lblStatus->setText("Added 10 test entries to database");
}

void MainWindow::loadDataFromDatabase()
{
    if (m_repository && DatabaseManager::instance().isOpen()) {
        QList<PasswordEntry> entries = m_repository->loadAll();
        m_tableModel->setEntries(entries);
        updateStatusBar();
        ui->lblStatus->setText(QString("Loaded %1 entries from database").arg(entries.size()));
    }
}

void MainWindow::saveEntryToDatabase(const PasswordEntry &entry)
{
    if (!m_repository || !DatabaseManager::instance().isOpen()) {
        showErrorMessage("Database Error", "Cannot save: database is not available");
        return;
    }

    if (m_repository->insert(entry)) {
        ui->lblStatus->setText("Entry saved to database");
    } else {
        showErrorMessage("Save Error", "Failed to save entry to database");
    }
}

void MainWindow::updateEntryInDatabase(const PasswordEntry &entry)
{
    if (!m_repository || !DatabaseManager::instance().isOpen()) {
        showErrorMessage("Database Error", "Cannot update: database is not available");
        return;
    }

    if (m_repository->update(entry)) {
        ui->lblStatus->setText("Entry updated in database");
    } else {
        showErrorMessage("Update Error", "Failed to update entry in database");
    }
}

void MainWindow::deleteEntryFromDatabase(int id)
{
    if (!m_repository || !DatabaseManager::instance().isOpen()) {
        showErrorMessage("Database Error", "Cannot delete: database is not available");
        return;
    }

    if (m_repository->remove(id)) {
        ui->lblStatus->setText("Entry deleted from database");
    } else {
        showErrorMessage("Delete Error", "Failed to delete entry from database");
    }
}

void MainWindow::showErrorMessage(const QString &title, const QString &message)
{
    QMessageBox::critical(this, title, message);
    qDebug() << title << ":" << message;
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
        "QPushButton:hover { background-color: #90caf9; }"
        "QPushButton#clearButton { background-color: #ffebee; border-color: #ef9a9a; color: #c62828; }"
        "QPushButton#clearButton:hover { background-color: #ffcdd2; }"
        "QTableView { background-color: white; alternate-background-color: #f8fbff; gridline-color: #e3f2fd; border: 1px solid #bbdef5; }"
        "QTableView::item:selected { background-color: #90caf9; color: #0d47a1; }"
        "QHeaderView::section { background-color: #e3f2fd; padding: 6px; border: none; border-right: 1px solid #bbdef5; color: #1565c0; font-weight: bold; }"
        "QStatusBar { background-color: #e3f2fd; color: #1565c0; }"
        "QLabel { color: #1565c0; }";

    setStyleSheet(styleSheet);
    if (ui->lblStatus) ui->lblStatus->setText("Light theme applied");
}

void MainWindow::applyDarkTheme()
{
    QString styleSheet =
        "QMainWindow { background-color: #2c2b26; }"
        "QMenuBar { background-color: #3d3b35; color: #f5d742; border-bottom: 1px solid #5a564c; }"
        "QMenuBar::item:selected { background-color: #5a564c; color: #ffea4a; }"
        "QMenu { background-color: #3d3b35; border: 1px solid #5a564c; color: #f5d742; }"
        "QMenu::item:selected { background-color: #5a564c; color: #ffea4a; }"
        "QToolBar { background-color: #3d3b35; border-bottom: 1px solid #5a564c; }"
        "QToolButton { color: #f5d742; }"
        "QToolButton:hover { background-color: #5a564c; color: #ffea4a; }"
        "QComboBox { background-color: #2c2b26; border: 1px solid #5a564c; color: #f5d742; }"
        "QLineEdit { background-color: #2c2b26; border: 1px solid #5a564c; color: #f5d742; }"
        "QPushButton { background-color: #3d3b35; border: 1px solid #5a564c; color: #f5d742; }"
        "QPushButton:hover { background-color: #5a564c; color: #ffea4a; }"
        "QTableView { background-color: #2c2b26; alternate-background-color: #33312c; gridline-color: #4a4842; color: #f5d742; }"
        "QTableView::item:selected { background-color: #5a564c; color: #ffea4a; }"
        "QHeaderView::section { background-color: #3d3b35; color: #f5d742; }"
        "QStatusBar { background-color: #3d3b35; color: #f5d742; }"
        "QLabel { color: #f5d742; }";

    setStyleSheet(styleSheet);
    if (ui->lblStatus) ui->lblStatus->setText("Dark theme applied");
}

void MainWindow::setupTableColumns()
{
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
    PasswordEntry newEntry;
    newEntry.title = "New Entry";
    newEntry.username = "";
    newEntry.password = "";
    newEntry.website = "";
    newEntry.category = ui->comboBox->currentText() != "All" ? ui->comboBox->currentText() : "General";
    newEntry.updatedAt = PasswordEntry::getCurrentTimestamp();

    // Зберігаємо в базу даних
    if (m_repository && DatabaseManager::instance().isOpen()) {
        if (m_repository->insert(newEntry)) {
            int newId = m_repository->getLastInsertId();
            newEntry.id = newId;
            m_tableModel->addEntry(newEntry);

            int newRow = m_tableModel->rowCount() - 1;
            QModelIndex editIndex = m_tableModel->index(newRow, 1);
            ui->tableViewAccounts->setCurrentIndex(editIndex);
            ui->tableViewAccounts->edit(editIndex);

            updateStatusBar();
            ui->lblStatus->setText("New entry created and saved to database");
        } else {
            showErrorMessage("Database Error", "Failed to create new entry");
        }
    } else {
        m_tableModel->addEntry(newEntry);
        int newRow = m_tableModel->rowCount() - 1;
        QModelIndex editIndex = m_tableModel->index(newRow, 1);
        ui->tableViewAccounts->setCurrentIndex(editIndex);
        ui->tableViewAccounts->edit(editIndex);
        updateStatusBar();
        ui->lblStatus->setText("New entry created (offline mode)");
    }
}

void MainWindow::onEdit()
{
    QModelIndex current = ui->tableViewAccounts->currentIndex();
    if (current.isValid()) {
        ui->tableViewAccounts->edit(current);
        ui->lblStatus->setText("Editing entry");
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

    PasswordEntry entry = m_tableModel->getEntryAt(current.row());

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Record",
        QString("Are you sure you want to delete entry '%1'?").arg(entry.title),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (entry.id > 0 && m_repository && DatabaseManager::instance().isOpen()) {
            deleteEntryFromDatabase(entry.id);
        }
        m_tableModel->removeEntry(current.row());
        updateStatusBar();
        ui->lblStatus->setText("Entry deleted");
    }
}

void MainWindow::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);

    PasswordEntry entry = m_tableModel->getEntryAt(topLeft.row());

    if (entry.id > 0) {
        entry.updatedAt = PasswordEntry::getCurrentTimestamp();
        updateEntryInDatabase(entry);
        QModelIndex dateIndex = m_tableModel->index(topLeft.row(), 6);
        emit m_tableModel->dataChanged(dateIndex, dateIndex);
    } else if (entry.id == 0 && entry.title != "New Entry") {
        entry.updatedAt = PasswordEntry::getCurrentTimestamp();
        saveEntryToDatabase(entry);
    }

    updateStatusBar();
}

void MainWindow::onCopyUsername()
{
    QModelIndex current = ui->tableViewAccounts->currentIndex();
    if (current.isValid()) {
        QString username = m_tableModel->data(m_tableModel->index(current.row(), 2)).toString();
        if (!username.isEmpty()) {
            QGuiApplication::clipboard()->setText(username);
            ui->lblStatus->setText("Username copied: " + username);
        } else {
            ui->lblStatus->setText("No username to copy");
        }
    } else {
        QMessageBox::information(this, "Copy", "Please select an entry");
    }
}

void MainWindow::onCopyPassword()
{
    QModelIndex current = ui->tableViewAccounts->currentIndex();
    if (current.isValid()) {
        QString password = m_tableModel->data(m_tableModel->index(current.row(), 3)).toString();
        if (!password.isEmpty()) {
            QGuiApplication::clipboard()->setText(password);
            ui->lblStatus->setText("Password copied");
        } else {
            ui->lblStatus->setText("No password to copy");
        }
    } else {
        QMessageBox::information(this, "Copy", "Please select an entry");
    }
}

void MainWindow::onSave()
{
    if (m_repository && DatabaseManager::instance().isOpen()) {
        QMessageBox::information(this, "Save", "All changes are automatically saved to the database");
        ui->lblStatus->setText("Database is up to date");
    } else {
        QMessageBox::warning(this, "Save", "Database is not available. Changes are only in memory.");
    }
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onGenerate()
{
    QMessageBox::information(this, "Generate Password",
                             "Password generator will be implemented in future practical works");
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Password Manager",
                       "Password Manager v2.0\n\n"
                       "A secure password management application\n"
                       "Built with Qt Framework\n\n"
                       "Features:\n"
                       "• SQLite database storage\n"
                       "• CRUD operations with database\n"
                       "• In-place cell editing\n"
                       "• Search and filter entries\n"
                       "• Light & Dark themes\n\n"
                       "Practical Work No.17 - SQLite Integration");
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    filterTable(text, ui->comboBox->currentText());
    if (ui->lblStatus) {
        ui->lblStatus->setText(text.isEmpty() ? "Search cleared" : "Searching: " + text);
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
    Q_UNUSED(index);
    filterTable(ui->searchEdit->text(), ui->comboBox->currentText());
    if (ui->lblStatus) ui->lblStatus->setText("Filtering by category: " + ui->comboBox->currentText());
}

void MainWindow::filterTable(const QString &searchText, const QString &category)
{
    for (int i = 0; i < m_tableModel->rowCount(); i++) {
        bool showRow = true;

        if (category != "All") {
            QString itemCategory = m_tableModel->data(m_tableModel->index(i, 5)).toString();
            if (itemCategory != category) showRow = false;
        }

        if (showRow && !searchText.isEmpty()) {
            QString title = m_tableModel->data(m_tableModel->index(i, 1)).toString();
            QString username = m_tableModel->data(m_tableModel->index(i, 2)).toString();
            QString website = m_tableModel->data(m_tableModel->index(i, 4)).toString();

            if (!title.contains(searchText, Qt::CaseInsensitive) &&
                !username.contains(searchText, Qt::CaseInsensitive) &&
                !website.contains(searchText, Qt::CaseInsensitive)) {
                showRow = false;
            }
        }

        ui->tableViewAccounts->setRowHidden(i, !showRow);
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    if (ui->lblTotal && ui->lblFiltered) {
        int total = m_tableModel->rowCount();
        ui->lblTotal->setText(QString("Total: %1").arg(total));

        int visible = 0;
        for (int i = 0; i < total; i++) {
            if (!ui->tableViewAccounts->isRowHidden(i)) visible++;
        }
        ui->lblFiltered->setText(QString("Filtered: %1").arg(visible));
    }
}

void MainWindow::onThemeLightBlue()
{
    applyLightTheme();
}

void MainWindow::onThemeDarkYellow()
{
    applyDarkTheme();
}