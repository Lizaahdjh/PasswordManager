#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DatabaseManager.h"
#include "PasswordRepository.h"
#include "PasswordLeakChecker.h"
#include <QMessageBox>
#include <QGuiApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QInputDialog>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_tableModel(nullptr)
    , m_proxyModel(nullptr)
    , m_repository(nullptr)
    , m_leakChecker(nullptr)
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
    m_tableModel->setRepository(m_repository);
    m_proxyModel = new PasswordFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);

    m_leakChecker = new PasswordLeakChecker(this);
    m_leakChecker->setTestMode(true);

    ui->tableViewAccounts->setModel(m_proxyModel);
    setupTableColumns();
    setupConnections();

    loadDataFromDatabase();

    if (m_tableModel->rowCount() == 0) {
        addTestDataToDatabase();
        loadDataFromDatabase();
    }

    if (ui->progressBar) {
        ui->progressBar->setVisible(false);
    }

    applyLightTheme();
    updateEmptyState();
}

MainWindow::~MainWindow()
{
    DatabaseManager::instance().close();
    delete ui;
}

int MainWindow::getCurrentSourceRow() const
{
    QModelIndex proxyIndex = ui->tableViewAccounts->currentIndex();
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    return sourceIndex.row();
}

PasswordEntry MainWindow::getCurrentEntry() const
{
    int sourceRow = getCurrentSourceRow();
    if (sourceRow >= 0 && sourceRow < m_tableModel->rowCount()) {
        return m_tableModel->getEntryAt(sourceRow);
    }
    return PasswordEntry();
}

void MainWindow::loadDataFromDatabase()
{
    if (m_repository && DatabaseManager::instance().isOpen()) {
        QList<PasswordEntry> entries = m_repository->loadAll();
        m_tableModel->setEntries(entries);
        updateStatusBar();
        ui->lblStatus->setText(QString("Loaded %1 entries from database").arg(entries.size()));
    }
    updateEmptyState();
}

void MainWindow::addTestDataToDatabase()
{
    if (!m_repository || !DatabaseManager::instance().isOpen()) {
        qDebug() << "Cannot add test data: database not available";
        return;
    }

    QString currentDate = PasswordEntry::getCurrentTimestamp();

    QList<PasswordEntry> testEntries = {
        {0, "Google Account", "user@gmail.com", "google_pass_2024", "https://google.com", "Logins", currentDate},
        {0, "GitHub", "developer@github.com", "ghp_token_abc123xyz", "https://github.com", "Development", currentDate},
        {0, "Facebook", "user@facebook.com", "password", "https://facebook.com", "Social Media", currentDate},
        {0, "Privat24", "+380501234567", "bank_pass_123", "https://privat24.ua", "Banking", currentDate},
        {0, "Netflix", "user@netflix.com", "netflix_2024", "https://netflix.com", "Entertainment", currentDate},
        {0, "Amazon", "buyer@amazon.com", "amazon_pass_2024", "https://amazon.com", "Shopping", currentDate},
        {0, "Twitter (X)", "user@twitter.com", "123456", "https://twitter.com", "Social Media", currentDate},
        {0, "LinkedIn", "professional@linkedin.com", "linkedin_2024", "https://linkedin.com", "Professional", currentDate},
        {0, "Home Wi-Fi", "Admin", "wifi_password_123", "192.168.1.1", "WiFi", currentDate},
        {0, "Windows 11 License", "user@microsoft.com", "WIN-XXXXX-XXXXX-XXXXX", "https://microsoft.com", "Software Licenses", currentDate}
    };

    for (const auto &entry : testEntries) {
        m_repository->insert(entry);
    }

    qDebug() << "Added" << testEntries.size() << "test entries to database";
}

void MainWindow::reloadTable()
{
    loadDataFromDatabase();
    m_proxyModel->refreshFilter();
    updateStatusBar();
    updateEmptyState();
    ui->lblStatus->setText("Table reloaded from database");
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
        "QLabel { color: #1565c0; }"
        "QProgressBar {"
        "    background-color: #e3f2fd;"
        "    border: 1px solid #bbdef5;"
        "    border-radius: 4px;"
        "    text-align: center;"
        "    color: #1565c0;"
        "    font-size: 11px;"
        "    height: 20px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #42a5f5;"
        "    border-radius: 3px;"
        "    margin: 1px;"
        "}"
        "QProgressBar:indeterminate::chunk {"
        "    background-color: #90caf9;"
        "    width: 20px;"
        "}";

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
        "QLabel { color: #f5d742; }"
        "QProgressBar {"
        "    background-color: #3d3b35;"
        "    border: 1px solid #5a564c;"
        "    border-radius: 4px;"
        "    text-align: center;"
        "    color: #f5d742;"
        "    font-size: 11px;"
        "    height: 20px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #f5d742;"
        "    border-radius: 3px;"
        "    margin: 1px;"
        "}"
        "QProgressBar:indeterminate::chunk {"
        "    background-color: #ffea4a;"
        "    width: 20px;"
        "}";

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
    ui->tableViewAccounts->setAlternatingRowColors(true);
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
    if (ui->actionCheck_Password) connect(ui->actionCheck_Password, &QAction::triggered, this, &MainWindow::onCheckPassword);

    if (ui->searchEdit) connect(ui->searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    if (ui->clearButton) connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearSearch);
    if (ui->comboBox) connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onCategoryChanged);

    if (m_leakChecker) {
        connect(m_leakChecker, &PasswordLeakChecker::checkStarted, this, &MainWindow::onLeakCheckStarted);
        connect(m_leakChecker, &PasswordLeakChecker::checkCompleted, this, &MainWindow::onLeakCheckCompleted);
        connect(m_leakChecker, &PasswordLeakChecker::checkFailed, this, &MainWindow::onLeakCheckFailed);
    }
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

    if (m_repository && DatabaseManager::instance().isOpen()) {
        if (m_repository->insert(newEntry)) {
            int newId = m_repository->getLastInsertId();
            newEntry.id = newId;
            m_tableModel->addEntry(newEntry);

            m_proxyModel->refreshFilter();

            int newRow = -1;
            for (int i = 0; i < m_proxyModel->rowCount(); i++) {
                QModelIndex proxyIndex = m_proxyModel->index(i, 1);
                if (m_proxyModel->data(proxyIndex).toString() == "New Entry") {
                    newRow = i;
                    break;
                }
            }

            if (newRow >= 0) {
                QModelIndex editIndex = m_proxyModel->index(newRow, 1);
                ui->tableViewAccounts->setCurrentIndex(editIndex);
                ui->tableViewAccounts->edit(editIndex);
            }

            updateStatusBar();
            ui->lblStatus->setText("New entry created and saved to database");
        } else {
            showErrorMessage("Database Error", "Failed to create new entry");
        }
    }
    updateEmptyState();
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

    PasswordEntry entry = getCurrentEntry();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Record",
        QString("Are you sure you want to delete entry '%1'?").arg(entry.title),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (entry.id > 0 && m_repository && DatabaseManager::instance().isOpen()) {
            if (m_repository->remove(entry.id)) {
                m_tableModel->removeEntry(getCurrentSourceRow());
                ui->lblStatus->setText("Entry deleted from database");
            } else {
                showErrorMessage("Delete Error", "Failed to delete entry from database");
            }
        } else {
            m_tableModel->removeEntry(getCurrentSourceRow());
            ui->lblStatus->setText("Entry deleted");
        }
        updateStatusBar();
        updateEmptyState();
    }
}

void MainWindow::onCopyUsername()
{
    PasswordEntry entry = getCurrentEntry();
    if (entry.id > 0) {
        if (!entry.username.isEmpty()) {
            QGuiApplication::clipboard()->setText(entry.username);
            ui->lblStatus->setText("Username copied: " + entry.username);
        } else {
            ui->lblStatus->setText("No username to copy");
        }
    } else {
        QMessageBox::information(this, "Copy", "Please select an entry");
    }
}

void MainWindow::onCopyPassword()
{
    PasswordEntry entry = getCurrentEntry();
    if (entry.id > 0) {
        if (!entry.password.isEmpty()) {
            QGuiApplication::clipboard()->setText(entry.password);
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
    PasswordEntry entry = getCurrentEntry();
    if (entry.id <= 0) {
        QMessageBox::information(this, "Generate Password",
                                 "Please select an entry first to generate password for.");
        return;
    }

    bool ok;
    int length = QInputDialog::getInt(this, "Generate Password",
                                      "Password length:", 16, 8, 64, 1, &ok);

    if (!ok) return;

    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "abcdefghijklmnopqrstuvwxyz"
                          "0123456789"
                          "!@#$%^&*()";

    QString newPassword;

    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    for (int i = 0; i < length; i++) {
        int index = std::rand() % chars.length();
        newPassword += chars[index];
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Generate Password",
        QString("Generate a new password for '%1'?\n\nNew password: %2\n\nThe password will be automatically saved.")
            .arg(entry.title).arg(newPassword),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        entry.password = newPassword;
        entry.updatedAt = PasswordEntry::getCurrentTimestamp();

        if (m_repository && m_repository->update(entry)) {
            int sourceRow = getCurrentSourceRow();
            m_tableModel->updateEntry(sourceRow, entry);

            ui->lblStatus->setText(QString("New %1-character password generated for '%2'")
                                       .arg(length).arg(entry.title));

            QMessageBox::information(this, "Success",
                                     QString("Password for '%1' has been updated successfully!")
                                         .arg(entry.title));
        } else {
            QMessageBox::warning(this, "Error", "Failed to save the new password to database.");
        }
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Password Manager",
                       "Password Manager v4.0\n\n"
                       "A secure password management application\n"
                       "Built with Qt Framework\n\n"
                       "Features:\n"
                       "• SQLite database storage\n"
                       "• Model/View architecture\n"
                       "• Search and filter with QSortFilterProxyModel\n"
                       "• In-place cell editing\n"
                       "• Password breach checking via Pwned Passwords API\n"
                       "• Password generator\n"
                       "• Light & Dark themes\n\n"
                       "Practical Work No.19 - Network Requests with QNetworkAccessManager");
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    m_proxyModel->setSearchText(text);
    updateStatusBar();
    updateEmptyState();
    ui->lblStatus->setText(text.isEmpty() ? "Search cleared" : "Searching: " + text);
}

void MainWindow::onClearSearch()
{
    if (ui->searchEdit) {
        ui->searchEdit->clear();
        m_proxyModel->setSearchText("");
        updateStatusBar();
        updateEmptyState();
        ui->lblStatus->setText("Search cleared");
    }
}

void MainWindow::onCategoryChanged(int index)
{
    Q_UNUSED(index);
    QString category = ui->comboBox->currentText();
    m_proxyModel->setCategoryFilter(category);
    updateStatusBar();
    updateEmptyState();
    ui->lblStatus->setText("Filtering by category: " + category);
}

void MainWindow::onResetFilters()
{
    ui->searchEdit->clear();
    ui->comboBox->setCurrentIndex(0);
    m_proxyModel->clearFilters();
    updateStatusBar();
    updateEmptyState();
    ui->lblStatus->setText("All filters cleared");
}

void MainWindow::onToggleTestMode()
{
    if (m_leakChecker) {
        bool newMode = !m_leakChecker->isTestMode();
        m_leakChecker->setTestMode(newMode);
        ui->lblStatus->setText(newMode ? "Test mode: Local password check only" : "Real API mode (requires internet)");

        QMessageBox::information(this, "Mode Changed",
                                 newMode ? "Now in TEST MODE - checking against local weak password list.\n\n"
                                           "Compromised test passwords include: password, 123456, qwerty, admin"
                                         : "Now in REAL API MODE - checking against HaveIBeenPwned database.\n\n"
                                           "Requires internet connection and SSL support.");
    }
}

void MainWindow::onCheckPassword()
{
    PasswordEntry entry = getCurrentEntry();
    if (entry.id <= 0) {
        QMessageBox::information(this, "Check Password", "Please select an entry first");
        return;
    }

    if (entry.password.isEmpty()) {
        QMessageBox::information(this, "Check Password", "Selected entry has no password");
        return;
    }

    if (m_leakChecker && m_leakChecker->isChecking()) {
        ui->lblStatus->setText("Already checking a password, please wait...");
        return;
    }

    ui->lblStatus->setText(QString("Checking password for '%1'...").arg(entry.title));
    m_leakChecker->checkPassword(entry.password);
}

void MainWindow::onLeakCheckStarted()
{
    if (ui->actionCheck_Password) {
        ui->actionCheck_Password->setEnabled(false);
    }
    if (ui->progressBar) {
        ui->progressBar->setVisible(true);
        ui->progressBar->setRange(0, 0);
    }
    if (ui->passwordCheckStatus) {
        ui->passwordCheckStatus->setText("Checking password...");
        ui->passwordCheckStatus->setStyleSheet("color: #ff9800;");
    }
    ui->lblStatus->setText("Checking password against breach database...");
}

void MainWindow::onLeakCheckCompleted(bool isLeaked, int breachCount)
{
    if (ui->actionCheck_Password) {
        ui->actionCheck_Password->setEnabled(true);
    }
    if (ui->progressBar) {
        ui->progressBar->setVisible(false);
        ui->progressBar->setRange(0, 100);
    }

    PasswordEntry entry = getCurrentEntry();

    if (isLeaked) {
        if (ui->passwordCheckStatus) {
            ui->passwordCheckStatus->setText(QString(" COMPROMISED! Found in %1 breaches").arg(breachCount));
            ui->passwordCheckStatus->setStyleSheet("color: #f44336; font-weight: bold;");
        }
        QMessageBox::warning(this, "Security Alert",
                             QString(" PASSWORD COMPROMISED!\n\n"
                                     "Password '%1' has been found in %2 data breaches!\n\n"
                                     "This password appears in leaked password databases.\n"
                                     "Change it immediately!")
                                 .arg(entry.password).arg(breachCount));
        ui->lblStatus->setText(" Password is compromised!");
    } else {
        if (ui->passwordCheckStatus) {
            ui->passwordCheckStatus->setText(" Safe - Not found in any breaches");
            ui->passwordCheckStatus->setStyleSheet("color: #4caf50;");
        }
        QMessageBox::information(this, "Password Check",
                                 QString("✓ PASSWORD IS SAFE!\n\n"
                                         "Password '%1' was not found in any known data breaches.\n\n"
                                         "However, always use unique strong passwords for each account.")
                                     .arg(entry.password));
        ui->lblStatus->setText("✓ Password is safe");
    }
}

void MainWindow::onLeakCheckFailed(const QString &errorMessage)
{
    if (ui->actionCheck_Password) {
        ui->actionCheck_Password->setEnabled(true);
    }
    if (ui->progressBar) {
        ui->progressBar->setVisible(false);
    }
    if (ui->passwordCheckStatus) {
        ui->passwordCheckStatus->setText(QString(" Check failed: %1").arg(errorMessage));
        ui->passwordCheckStatus->setStyleSheet("color: #f44336;");
    }
    ui->lblStatus->setText("Check failed: " + errorMessage);
    QMessageBox::warning(this, "Check Failed",
                         QString("Password check failed:\n%1\n\n"
                                 "Check your internet connection and try again.")
                             .arg(errorMessage));
}

void MainWindow::updateStatusBar()
{
    if (ui->lblTotal && ui->lblFiltered) {
        int total = m_tableModel->rowCount();
        int visible = m_proxyModel->rowCount();
        ui->lblTotal->setText(QString("Total: %1").arg(total));
        ui->lblFiltered->setText(QString("Filtered: %1").arg(visible));
    }
}

void MainWindow::updateEmptyState()
{
    bool isEmpty = (m_proxyModel->rowCount() == 0);
    bool hasSearch = !ui->searchEdit->text().isEmpty();
    bool hasCategory = ui->comboBox->currentText() != "All";

    if (isEmpty && hasSearch && hasCategory) {
        ui->lblStatus->setText("No results found for \"" + ui->searchEdit->text() +
                               "\" in category \"" + ui->comboBox->currentText() + "\"");
    } else if (isEmpty && hasSearch) {
        ui->lblStatus->setText("No results found for \"" + ui->searchEdit->text() + "\"");
    } else if (isEmpty && hasCategory) {
        ui->lblStatus->setText("No entries in category \"" + ui->comboBox->currentText() + "\"");
    } else if (isEmpty) {
        ui->lblStatus->setText("No entries. Click 'New Entry' to add.");
    } else if (!hasSearch && !hasCategory) {
        ui->lblStatus->setText(QString("Showing %1 entries").arg(m_proxyModel->rowCount()));
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