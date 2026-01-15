#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QVector>
#include <QCloseEvent>
#include "QSO.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateDateTime();
    void on_Push2Log_clicked();
    void on_RealTime_toggled(bool checked);
    void on_timeEdit_timeChanged(const QTime &time);
    void on_dateEdit_dateChanged(const QDate &date);
    void on_ExportAdif_2_triggered();
    void on_FilterDate_clicked();
    void on_ClearFilter_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    int rowcount = 0;
    QVector<TQSO> QSOHystory;
    QString operatorCall;

    const QString logFileName = "autosave_log.csv";
    void saveLogToFile(QString fileName);
    void loadLogFromFile(QString fileName);

    QVector<TQSO> originalData; // Хранилище всех данных для сброса фильтра
    bool isFiltered = false;


    void Array2Table();
    void ExportToADIF();
};
#endif
