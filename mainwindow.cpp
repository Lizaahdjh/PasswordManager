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
    , m_proxyModel(nullptr)
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
    m_tableModel->setRepository(m_repository);

    m_proxyModel = new PasswordFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);

    ui->tableViewAccounts->setModel(m_proxyModel);
    setupTableColumns();
    setupConnections();

    loadDataFromDatabase();

    if (m_tableModel->rowCount() == 0) {
        addTestDataToDatabase();
        loadDataFromDatabase();
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
        {0, "Facebook", "user@facebook.com", "fb_password_2024", "https://facebook.com", "Social Media", currentDate},
        {0, "Privat24", "+380501234567", "bank_pass_123", "https://privat24.ua", "Banking", currentDate},
        {0, "Netflix", "user@netflix.com", "netflix_2024", "https://netflix.com", "Entertainment", currentDate},
        {0, "Amazon", "buyer@amazon.com", "amazon_pass_2024", "https://amazon.com", "Shopping", currentDate},
        {0, "Twitter (X)", "user@twitter.com", "twitter_pass_2024", "https://twitter.com", "Social Media", currentDate},
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
    QMessageBox::information(this, "Generate Password",
                             "Password generator will be implemented in future practical works");
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Password Manager",
                       "Password Manager v3.0\n\n"
                       "A secure password management application\n"
                       "Built with Qt Framework\n\n"
                       "Features:\n"
                       "• SQLite database storage\n"
                       "• Model/View architecture\n"
                       "• Search and filter with QSortFilterProxyModel\n"
                       "• In-place cell editing\n"
                       "• Light & Dark themes\n\n"
                       "Practical Work No.18 - Model/View, Search and Filter");
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
    ui->comboBox->setCurrentIndex(0); // "All"
    m_proxyModel->clearFilters();
    updateStatusBar();
    updateEmptyState();
    ui->lblStatus->setText("All filters cleared");
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