#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include "QSO.h"
#include <QMessageBox>

TQSO QSO;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();

    timer = new QTimer(this);

    ui->QSOTable->setColumnWidth(0, 120);
    ui->QSOTable->setColumnWidth(1, 80);
    ui->QSOTable->setColumnWidth(2, 150);
    ui->QSOTable->setColumnWidth(3, 70);
    ui->QSOTable->setColumnWidth(4, 70);
    ui->QSOTable->setColumnWidth(5, 70);
    ui->QSOTable->setColumnWidth(6, 70);
    ui->QSOTable->setColumnWidth(7, 203);
    ui->QSOTable->setColumnWidth(8, 210);
    ui->QSOTable->setColumnWidth(9, 250);

    ui->timeEdit->setTime(QTime::currentTime());
    connect(timer, &QTimer::timeout, this, &MainWindow::updateDateTime);

    connect(ui->RealTime, &QCheckBox::toggled, this, &MainWindow::on_RealTime_toggled);

    ui->dateEdit->setDate(QDate::currentDate());
    connect(ui->RealTime, &QCheckBox::toggled, this, &MainWindow::on_RealTime_toggled);



}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDateTime()
{


    ui->timeEdit->setTime(QTime::currentTime());
    ui->dateEdit->setDate(QDate::currentDate());
    // ui->QSOTable->item(3,2)->setText();


}



void MainWindow::on_action_triggered()
{
    SettingsWindow settings;
    settings.setModal(true);
    settings.exec();
}





void MainWindow::on_RealTime_toggled(bool checked)
{
    if(checked){
        timer->start(1000);
        ui->timeEdit->setTime(QTime::currentTime());
        ui->dateEdit->setDate(QDate::currentDate());
    }
    else {
        timer->stop();
    }
}


void MainWindow::on_timeEdit_timeChanged(const QTime &time)
{
    QString strtime = time.toString("hh:mm");
    QTableWidgetItem *timeItem = ui->QSOTable->item(rowcount,1);
    if (!timeItem) {
        timeItem = new QTableWidgetItem();
        ui->QSOTable->setItem(rowcount, 1, timeItem);
    }
    timeItem->setText(strtime);
}


void MainWindow::on_dateEdit_dateChanged(const QDate &date)
{
    QString strdate = date.toString("yyyy-MM-dd");
    QTableWidgetItem *dateItem = ui->QSOTable->item(rowcount,0);
    if (!dateItem) {
        dateItem = new QTableWidgetItem();
        ui->QSOTable->setItem(rowcount, 0, dateItem);
    }
    dateItem->setText(strdate);
}

void MainWindow::on_Push2Log_clicked()
{

}
