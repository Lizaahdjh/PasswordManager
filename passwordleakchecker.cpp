#include "PasswordLeakChecker.h"
#include <QCryptographicHash>
#include <QUrl>
#include <QNetworkRequest>
#include <QDebug>
#include <QSslSocket>
#include <QRandomGenerator>

const QString PasswordLeakChecker::API_BASE_URL = "https://api.pwnedpasswords.com/range/";

PasswordLeakChecker::PasswordLeakChecker(QObject *parent)
    : QObject(parent)
    , m_networkManager(nullptr)
    , m_currentReply(nullptr)
    , m_timeoutTimer(nullptr)
    , m_isChecking(false)
    , m_testMode(true)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &PasswordLeakChecker::onTimeout);
}

PasswordLeakChecker::~PasswordLeakChecker()
{
    cleanup();
}

void PasswordLeakChecker::performTestCheck(const QString &password)
{
    static const QSet<QString> compromisedPasswords = {
        "password", "123456", "qwerty", "admin", "welcome",
        "password123", "123456789", "111111", "123123",
        "abc123", "iloveyou", "admin123", "root", "passw0rd"
    };

    QString lowerPassword = password.toLower();
    bool isLeaked = compromisedPasswords.contains(lowerPassword);
    int breachCount = isLeaked ? QRandomGenerator::global()->bounded(100, 10000) : 0;

    qDebug() << "=== TEST MODE ===";
    qDebug() << "Password:" << password;
    qDebug() << "Is leaked:" << isLeaked;
    qDebug() << "Breach count:" << breachCount;

    emit checkStarted();

    QTimer::singleShot(500, this, [this, isLeaked, breachCount]() {
        emit checkCompleted(isLeaked, breachCount);
        m_isChecking = false;
    });
}

void PasswordLeakChecker::checkPassword(const QString &password)
{
    if (m_isChecking) {
        cancelCheck();
    }

    if (password.isEmpty()) {
        emit checkFailed("Password is empty");
        return;
    }

    if (m_testMode) {
        performTestCheck(password);
        return;
    }

    qDebug() << "=== REAL API CHECK ===";
    qDebug() << "Password:" << password;

    QString fullHash = computeSha1Hash(password);
    qDebug() << "Full SHA1 hash:" << fullHash;

    m_currentPasswordHash = fullHash;
    QString hashPrefix = fullHash.left(5);
    qDebug() << "Prefix (5 chars):" << hashPrefix;

    QString urlString = API_BASE_URL + hashPrefix;
    QUrl url(urlString);
    qDebug() << "URL:" << urlString;

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "PasswordManager/1.0");
    request.setRawHeader("Add-Padding", "true");
    request.setTransferTimeout(DEFAULT_TIMEOUT_MS);

    m_isChecking = true;
    m_currentReply = m_networkManager->get(request);

    connect(m_currentReply, &QNetworkReply::finished, this, &PasswordLeakChecker::onReplyFinished);
    connect(m_currentReply, &QNetworkReply::errorOccurred, this, &PasswordLeakChecker::onReplyErrorOccurred);

    m_timeoutTimer->start(DEFAULT_TIMEOUT_MS);

    emit checkStarted();
}

void PasswordLeakChecker::cancelCheck()
{
    cleanup();
    m_isChecking = false;
}

void PasswordLeakChecker::onReplyFinished()
{
    if (!m_currentReply) return;

    m_timeoutTimer->stop();

    int statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP Status Code:" << statusCode;

    if (statusCode != 200) {
        QString errorMsg = QString("HTTP Error: %1").arg(statusCode);
        emit checkFailed(errorMsg);
        cleanup();
        m_isChecking = false;
        return;
    }

    QByteArray responseData = m_currentReply->readAll();

    if (responseData.isEmpty()) {
        emit checkFailed("Empty response from server");
        cleanup();
        m_isChecking = false;
        return;
    }

    int breachCount = 0;
    bool found = parseResponse(responseData, m_currentPasswordHash, breachCount);

    emit checkCompleted(found, breachCount);

    cleanup();
    m_isChecking = false;
}

void PasswordLeakChecker::onReplyErrorOccurred(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    if (!m_currentReply) return;

    m_timeoutTimer->stop();

    QString errorString = m_currentReply->errorString();
    qDebug() << "Network error:" << errorString;

    emit networkError(errorString);
    emit checkFailed("Network error: " + errorString);

    cleanup();
    m_isChecking = false;
}

void PasswordLeakChecker::onTimeout()
{
    qDebug() << "Request timeout";

    if (m_currentReply && m_currentReply->isRunning()) {
        m_currentReply->abort();
    }

    emit timeoutOccurred();
    emit checkFailed("Request timeout - server did not respond");

    cleanup();
    m_isChecking = false;
}

QString PasswordLeakChecker::computeSha1Hash(const QString &password) const
{
    QByteArray utf8Password = password.toUtf8();
    QByteArray hash = QCryptographicHash::hash(utf8Password, QCryptographicHash::Sha1);
    return QString::fromLatin1(hash.toHex()).toUpper();
}

bool PasswordLeakChecker::parseResponse(const QByteArray &response, const QString &fullHash, int &breachCount) const
{
    breachCount = 0;

    QString fullHashSuffix = fullHash.mid(5);
    QString responseStr = QString::fromUtf8(response);
    QStringList lines = responseStr.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList parts = line.split(':', Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            QString suffix = parts[0].trimmed();
            int count = parts[1].trimmed().toInt();

            if (suffix.compare(fullHashSuffix, Qt::CaseInsensitive) == 0) {
                breachCount = count;
                qDebug() << "Password found! Count:" << count;
                return true;
            }
        }
    }

    qDebug() << "Password not found in breaches";
    return false;
}

void PasswordLeakChecker::cleanup()
{
    if (m_currentReply) {
        m_currentReply->disconnect();
        if (m_currentReply->isRunning()) {
            m_currentReply->abort();
        }
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    if (m_timeoutTimer && m_timeoutTimer->isActive()) {
        m_timeoutTimer->stop();
    }
}