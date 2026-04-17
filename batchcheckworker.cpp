#include "BatchCheckWorker.h"
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QRandomGenerator>

const QString BatchCheckWorker::API_BASE_URL = "https://api.pwnedpasswords.com/range/";

BatchCheckWorker::BatchCheckWorker(QObject *parent)
    : QObject(parent)
    , m_cancelled(false)
    , m_testMode(true)
{
}

void BatchCheckWorker::processAll(const QList<PasswordEntry> &entries)
{
    m_cancelled = false;
    BatchCheckResult result;
    result.total = entries.size();

    emit progressChanged(0, result.total);

    for (int i = 0; i < entries.size(); i++) {
        if (m_cancelled) {
            emit error("Batch check cancelled by user");
            return;
        }

        const PasswordEntry &entry = entries[i];

        if (entry.password.isEmpty()) {
            result.checked++;
            emit progressChanged(result.checked, result.total);
            continue;
        }

        int breachCount = 0;
        bool isCompromised = checkSinglePassword(entry.password, breachCount);

        result.checked++;

        if (isCompromised) {
            result.compromised++;
            result.compromisedIds.append(entry.id);
            result.breachCounts[entry.id] = breachCount;
        } else {
            result.failed++;
        }

        emit entryChecked(entry.id, isCompromised, breachCount);
        emit progressChanged(result.checked, result.total);

        QThread::msleep(100);
    }

    emit finished(result);
}

void BatchCheckWorker::cancel()
{
    m_cancelled = true;
}

bool BatchCheckWorker::checkSinglePassword(const QString &password, int &breachCount)
{
    breachCount = 0;

    if (m_testMode) {
        bool isLeaked;
        performTestCheck(password, isLeaked, breachCount);
        return isLeaked;
    }

    QString fullHash = computeSha1Hash(password);
    QString hashPrefix = fullHash.left(5);
    QString urlString = API_BASE_URL + hashPrefix;

    QNetworkAccessManager manager;
    QUrl url(urlString);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "PasswordManager/1.0");
    request.setRawHeader("Add-Padding", "true");
    request.setTransferTimeout(DEFAULT_TIMEOUT_MS);

    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(DEFAULT_TIMEOUT_MS);
    loop.exec();

    if (!timer.isActive()) {
        reply->abort();
        reply->deleteLater();
        return false;
    }
    timer.stop();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return false;
    }

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200) {
        reply->deleteLater();
        return false;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    return parseResponse(responseData, fullHash, breachCount);
}

QString BatchCheckWorker::computeSha1Hash(const QString &password) const
{
    QByteArray utf8Password = password.toUtf8();
    QByteArray hash = QCryptographicHash::hash(utf8Password, QCryptographicHash::Sha1);
    return QString::fromLatin1(hash.toHex()).toUpper();
}

bool BatchCheckWorker::parseResponse(const QByteArray &response, const QString &fullHash, int &breachCount)
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
                return true;
            }
        }
    }

    return false;
}

void BatchCheckWorker::performTestCheck(const QString &password, bool &isLeaked, int &breachCount)
{
    static const QSet<QString> compromisedPasswords = {
        "password", "123456", "qwerty", "admin", "welcome",
        "password123", "123456789", "111111", "123123",
        "abc123", "iloveyou", "admin123", "root", "passw0rd"
    };

    QString lowerPassword = password.toLower();
    isLeaked = compromisedPasswords.contains(lowerPassword);
    breachCount = isLeaked ? QRandomGenerator::global()->bounded(100, 10000) : 0;
}