#include <QAtomicInteger>
#include <QRandomGenerator>
#include <QThread>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto &tableWidget = *ui->tableWidget;

    tableWidget.insertColumn(0);
    tableWidget.insertColumn(1);
    tableWidget.insertColumn(2);
    tableWidget.insertColumn(3);

    auto &elaspedTimeItem = *new QTableWidgetItem();
    elaspedTimeItem.setText(tr("Elasped time"));

    auto &threadIdItem = *new QTableWidgetItem();
    threadIdItem.setText(tr("Thread Id"));

    auto &valueItem = *new QTableWidgetItem();
    valueItem.setText(tr("Value"));

    auto &workResultItem = *new QTableWidgetItem();
    workResultItem.setText(tr("Work result"));

    tableWidget.setColumnWidth(0, 200);
    tableWidget.setColumnWidth(1, 200);
    tableWidget.setColumnWidth(2, 200);
    tableWidget.setColumnWidth(3, 340);
    tableWidget.setHorizontalHeaderItem(0, &elaspedTimeItem);
    tableWidget.setHorizontalHeaderItem(1, &threadIdItem);
    tableWidget.setHorizontalHeaderItem(2, &valueItem);
    tableWidget.setHorizontalHeaderItem(3, &workResultItem);

    auto &horizontalHeader = *tableWidget.horizontalHeader();
    horizontalHeader.setSectionResizeMode(0, QHeaderView::Stretch);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_startButton_clicked() {
    const auto findNumber = ui->findNumberSpinBox->value();
    const auto maxDelay = ui->delaySpinBox->value();
    const auto maxIterCount = ui->iterSpinBox->value();

    abort.storeRelaxed(false);
    counter.storeRelaxed(0);
    timer.restart();
    ui->tableWidget->setRowCount(0);

    for (auto i = 0; i < ui->threadSpinBox->value(); i++) {
        auto &thread = *new QThread(parent());

        auto &worker = *new Worker(abort, counter, mutex, *ui->tableWidget, findNumber, maxDelay, maxIterCount, i, timer);
        worker.moveToThread(&thread);

        connect(&thread, SIGNAL(started()), &worker, SLOT(doWork()));
        connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
        connect(&worker, SIGNAL(isCompleted()), &thread, SLOT(quit()));

        thread.start();
    }
}

void MainWindow::on_stopButton_clicked() {
    abort.storeRelaxed(true);
}

