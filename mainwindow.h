#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QElapsedTimer>
#include <QMainWindow>

#include "worker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::MainWindow *ui;

    QAtomicInteger<bool> abort;
    QAtomicInteger<quint32> counter;
    QMutex mutex;
    QElapsedTimer timer;
};
#endif // MAINWINDOW_H
