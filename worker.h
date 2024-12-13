#ifndef WORKER_H
#define WORKER_H

#include <QAtomicInteger>
#include <QElapsedTimer>
#include <QMutex>
#include <QObject>
#include <QTableWidget>

enum WorkResult : quint8 {
    IsAborted,
    NumberInRange,
    MaxNumberOfIterationsReached,
};

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(
        QAtomicInteger<bool> &abort,
        QAtomicInteger<quint32> &counter,
        QMutex &mutex,
        QTableWidget &tableWidget,
        const quint32 findNumber,
        const quint32 maxDelay,
        const quint32 maxIterCount,
        const quint32 threadId,
        const QElapsedTimer &timer
    );

    void updateTable(const quint32, const double, const qsizetype, const WorkResult);

public slots:
    void doWork();

private:
    bool isAborted = false;
    QAtomicInteger<bool> &abort;
    QAtomicInteger<quint32> &counter;
    QMutex &mutex;
    const quint32 findNumber;
    const quint32 maxDelay;
    const quint32 maxIterCount;
    const quint32 threadId;
    const QElapsedTimer &timer;
    QTableWidget &tableWidget;

signals:
    void sleep(unsigned int);
    void isCompleted();
};

#endif // WORKER_H
