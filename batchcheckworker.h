#ifndef BATCHCHECKWORKER_H
#define BATCHCHECKWORKER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>
#include "PasswordEntry.h"

struct BatchCheckResult
{
    int total = 0;
    int checked = 0;
    int compromised = 0;
    int failed = 0;
    QList<int> compromisedIds;
    QHash<int, int> breachCounts;

    BatchCheckResult() = default;
};

class BatchCheckWorker : public QObject
{
    Q_OBJECT

public:
    explicit BatchCheckWorker(QObject *parent = nullptr);

    void setTestMode(bool enabled) { m_testMode = enabled; }
    bool isTestMode() const { return m_testMode; }

public slots:
    void processAll(const QList<PasswordEntry> &entries);
    void cancel();

signals:
    void progressChanged(int current, int total);
    void entryChecked(int entryId, bool isCompromised, int breachCount);
    void finished(const BatchCheckResult &result);
    void error(const QString &errorMessage);

private:
    bool checkSinglePassword(const QString &password, int &breachCount);
    QString computeSha1Hash(const QString &password) const;
    bool parseResponse(const QByteArray &response, const QString &fullHash, int &breachCount);
    void performTestCheck(const QString &password, bool &isLeaked, int &breachCount);

    bool m_cancelled;
    bool m_testMode;

    static const QString API_BASE_URL;
    static constexpr int DEFAULT_TIMEOUT_MS = 10000;
};

#endif // BATCHCHECKWORKER_H