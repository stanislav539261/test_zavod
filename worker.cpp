#include <QRandomGenerator>
#include <QThread>

#include "worker.h"

Worker::Worker(
    QAtomicInteger<bool> &abort,
    QAtomicInteger<quint32> &counter,
    QMutex &mutex,
    QTableWidget &tableWidget,
    const quint32 findNumber,
    const quint32 maxDelay,
    const quint32 maxIterCount,
    const quint32 threadId,
    const QElapsedTimer &timer
) : abort(abort),
    counter(counter),
    mutex(mutex),
    tableWidget(tableWidget),
    findNumber(findNumber),
    maxDelay(maxDelay),
    maxIterCount(maxIterCount),
    threadId(threadId),
    timer(timer) {};

void Worker::doWork() {
    constexpr auto MIN_DELAY = 1u;

    auto number = 0u;

    while (counter.fetchAndAddRelaxed(1) < maxIterCount) {
        number = QRandomGenerator::global()->bounded(1, 10);

        if (abort.loadRelaxed()) {
            updateTable(
                number,
                static_cast<double>(timer.elapsed()) * 0.001,
                threadId,
                WorkResult::IsAborted
            );
            return;
        }

        if (number == findNumber) {
            updateTable(
                number,
                static_cast<double>(timer.elapsed()) * 0.001,
                threadId,
                WorkResult::NumberInRange
            );
            return;
        }

        QThread::sleep(MIN_DELAY == maxDelay ? MIN_DELAY : QRandomGenerator::global()->bounded(MIN_DELAY, maxDelay));
    }

    updateTable(
        number,
        static_cast<double>(timer.elapsed()) * 0.001,
        threadId,
        WorkResult::MaxNumberOfIterationsReached
    );
}

void Worker::updateTable(
    const quint32 value,
    const double elaspedTime,
    const qsizetype threadId,
    const WorkResult workResult
) {
    mutex.lock();

    const auto nextRow = tableWidget.rowCount();

    tableWidget.insertRow(nextRow);

    auto &elaspedTimeItem = *new QTableWidgetItem();
    elaspedTimeItem.setText(QString::number(elaspedTime, 'f', 3));
    elaspedTimeItem.setFlags(Qt::ItemIsEnabled);

    auto &threadIdItem = *new QTableWidgetItem();
    threadIdItem.setText(QString::number(threadId));
    threadIdItem.setFlags(Qt::ItemIsEnabled);

    auto &valueItem = *new QTableWidgetItem();
    valueItem.setFlags(Qt::ItemIsEnabled);
    valueItem.setText(QString::number(value));

    auto &workResultItem = *new QTableWidgetItem();
    workResultItem.setFlags(Qt::ItemIsEnabled);

    switch (workResult) {
        case WorkResult::IsAborted:
            workResultItem.setText(tr("Is aborted"));
            break;
        case WorkResult::MaxNumberOfIterationsReached:
            workResultItem.setText(tr("Max number of iterations reached"));
            break;
        case WorkResult::NumberInRange:
            workResultItem.setText(tr("Number is found in range (1 - 10)"));
            break;
        default:
            break;
    }

    tableWidget.setItem(nextRow, 0, &elaspedTimeItem);
    tableWidget.setItem(nextRow, 1, &threadIdItem);
    tableWidget.setItem(nextRow, 2, &valueItem);
    tableWidget.setItem(nextRow, 3, &workResultItem);

    mutex.unlock();

    emit isCompleted();
}
