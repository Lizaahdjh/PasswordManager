#ifndef PASSWORDLEAKCHECKER_H
#define PASSWORDLEAKCHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class PasswordLeakChecker : public QObject
{
    Q_OBJECT

public:
    explicit PasswordLeakChecker(QObject *parent = nullptr);
    ~PasswordLeakChecker();

    void checkPassword(const QString &password);
    bool isChecking() const { return m_isChecking; }
    void cancelCheck();
    void setTestMode(bool enabled) { m_testMode = enabled; }
    bool isTestMode() const { return m_testMode; }

signals:
    void checkStarted();
    void checkCompleted(bool isLeaked, int breachCount);
    void checkFailed(const QString &errorMessage);
    void networkError(const QString &errorMessage);
    void timeoutOccurred();

private slots:
    void onReplyFinished();
    void onReplyErrorOccurred(QNetworkReply::NetworkError code);
    void onTimeout();

private:
    QString computeSha1Hash(const QString &password) const;
    void performTestCheck(const QString &password);
    bool parseResponse(const QByteArray &response, const QString &fullHash, int &breachCount) const;
    void cleanup();

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QTimer *m_timeoutTimer;
    QString m_currentPasswordHash;
    bool m_isChecking;
    bool m_testMode;

    static constexpr int DEFAULT_TIMEOUT_MS = 10000;
    static const QString API_BASE_URL;
};

#endif // PASSWORDLEAKCHECKER_H