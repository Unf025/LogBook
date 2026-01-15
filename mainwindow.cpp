#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QSO.h"
#include <QMessageBox>
#include <QVector>
#include <string>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QInputDialog>
#include <QSettings>
#include <clocale>
#include <string>
#include <algorithm>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->QSOTable->setRowCount(0);
    rowcount = 0;
    showMaximized();



    timer = new QTimer(this);
    ui->timeEdit->setTime(QTime::currentTime());
    ui->dateEdit->setDate(QDate::currentDate());

    connect(timer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    connect(ui->QTHIn, &QLineEdit::returnPressed, this, &MainWindow::on_Push2Log_clicked);
    loadLogFromFile(logFileName);

    QSettings settings("RA9HPLOG", "QSO_Logger");
    operatorCall = settings.value("operator", "").toString();

    if (operatorCall.isEmpty()) {
        bool ok;
        QString text = QInputDialog::getText(this,
                                             QString::fromUtf8("Первый запуск"),
                                             QString::fromUtf8("Введите ваш позывной (Operator):"),
                                             QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) {
            operatorCall = text.trimmed().toUpper();
            settings.setValue("operator", operatorCall);
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


bool compareQSOs(const TQSO &a, const TQSO &b) {
    if (a.date != b.date) {
        return a.date < b.date;
    }
    return a.time < b.time;
}

void MainWindow::Array2Table() {

    ui->QSOTable->setRowCount(0);

    std::sort(QSOHystory.begin(), QSOHystory.end(), compareQSOs);

    for (int i = 0; i < QSOHystory.size(); ++i) {
        const TQSO& qso = QSOHystory[i];
        ui->QSOTable->insertRow(i);

        ui->QSOTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(qso.date)));
        ui->QSOTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(qso.time)));
        ui->QSOTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(qso.callsign).toUpper()));
        ui->QSOTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(qso.band)));
        ui->QSOTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(qso.mode)));
        ui->QSOTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(qso.rstr)));
        ui->QSOTable->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(qso.rsts)));
        ui->QSOTable->setItem(i, 7, new QTableWidgetItem(QString::fromStdString(qso.name)));
        ui->QSOTable->setItem(i, 8, new QTableWidgetItem(QString::fromStdString(qso.qth)));
    }

    rowcount = QSOHystory.size();
}

void MainWindow::updateDateTime()
{
    ui->timeEdit->setTime(QTime::currentTime());
    ui->dateEdit->setDate(QDate::currentDate());
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

void MainWindow::on_Push2Log_clicked() {
    if (ui->CallsignIn->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Запись QSO"), QStringLiteral("Введите позывной!"));
        return;
    }


    TQSO newQso = {
        ui->dateEdit->date().toString("yyyy-MM-dd").toStdString(),
        ui->timeEdit->time().toString("hh:mm").toStdString(),
        ui->CallsignIn->text().toStdString(),
        ui->BandBox->currentText().toStdString(),
        ui->ModeBox->currentText().toStdString(),
        ui->RSTr->text().toStdString(),
        ui->RSTs->text().toStdString(),
        ui->NameIn->text().toStdString(),
        ui->QTHIn->text().toStdString()
    };

    QSOHystory.append(newQso);

    Array2Table();

    ui->CallsignIn->clear();
    ui->RSTr->clear();
    ui->RSTs->clear();
    ui->NameIn->clear();
    ui->QTHIn->clear();
}

void MainWindow::ExportToADIF()
{
    if (QSOHystory.empty()) {
        QMessageBox::warning(this, "Экспорт в ADIF", "Нет данных для экспорта!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранение лога в ADIF",
                                                    QDir::homePath() + "/QSO_log.adi",
                                                    "ADIF-файл» (*.adi *.adif);; Все файлы (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Не удалось открыть файл для записи!");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

\
    out << "Generated by RA9HP QSO Logger\n";
    out << "ADIF Export\n";
    out << "<ADIF_VER:5>3.1.0\n";
    out << "<CREATED_TIMESTAMP:15>" << QDateTime::currentDateTime().toString("yyyyMMdd hhmmss")<<"\n";
    out << "<PROGRAMID:11>QSO_Logger\n";
    out << "<PROGRAMVERSION:5>0.0.1\n";
    out << "<EOH>\n";


    for (const TQSO& qso : QSOHystory) {

        if (!operatorCall.isEmpty()) {

            out <<"<OPERATOR:" << operatorCall.length() << ">" << operatorCall
                <<"<CALL:"<< (qso.callsign).size()
                <<">"
                << QString::fromStdString(qso.callsign).toUpper()
                << "<QSO_DATE:"
                << (QString::fromStdString(qso.date).replace("-", "")).size()
                << ">"
                << QString::fromStdString(qso.date).replace("-", "")
                << "<TIME_ON:"
                << (QString::fromStdString(qso.time).replace(":", "")).size()
                << ">"
                << QString::fromStdString(qso.time).replace(":", "")
                << "<TIME_OFF:"
                << (QString::fromStdString(qso.time).replace(":", "")).size()
                << ">"
                << QString::fromStdString(qso.time).replace(":", "");

            QString band = QString::fromStdString(qso.band).toUpper();
            if (band.contains("1.8")) {
                out << "<BAND:4>160M"
                    << "<FREQ:5>1.800";
            }
            else if ( band.contains("3.5")) {
                out << "<BAND:3>80M"
                    << "<FREQ:5>3.500";
            }
            else if (band.contains("7")) {
                out << "<BAND:3>"
                    << "40M"
                    << "<FREQ:5>7.000";
            }
            else if (band.contains("14")) {
                out << "<BAND:3>20M"
                    << "<FREQ:6>14.000";
            }
            else if (band.contains("18")) {
                out << "<BAND:3>18M"
                    << "<FREQ:6>18.100";
            }
            else if (band.contains("21")) {
                out << "<BAND:3>15M"
                    << "<FREQ:6>21.000";
            }
            else if (band.contains("24")) {
                out << "<BAND:3>12M"
                    << "<FREQ:6>24.900";
            }
            else if (band.contains("28")) {
                out << "<BAND:3>10M"
                    << "<FREQ:6>28.000";
            }
            else if (band.contains("144")) {
                out << "<BAND:2>2M"
                    << "<FREQ:6>144.00";
            }
            else if (band.contains("430")) {
                out << "<BAND:4>70CM"
                    << "<FREQ:6>430.00";
            }
            else {
                out << "<BAND:" << band.length() << ">" << band ;
            }


            QString mode = QString::fromStdString(qso.mode).toUpper();
            if (mode == "SSB") {
                out << "<MODE:3>SSB";
            }
            else if (mode == "CW") {
                out << "<MODE:2>CW";
            }
            else if (mode == "FT8") {
                out << "<MODE:3>FT8";
            }
            else if (mode == "FT4") {
                out << "<MODE:3>FT4";
            }
            else if (mode == "RTTY") {
                out << "<MODE:4>RTTY";
            }
            else if (mode == "BPSK31") {
                out << "<MODE:6>BPSK31";
            }
            else if (mode == "BPSK63") {
                out << "<MODE:6>BPSK63";
            }
            else if (mode == "BPSK125") {
                out << "<MODE:7>BPSK125";
            }
            else if (mode == "FM") {
                out << "<MODE:2>FM";
            }
            else {
                out << "<MODE:" << mode.length() << ">" << mode ;
            }

            // RST
            if (!qso.rstr.empty()) {
                out << "<RST_RCVD:" << qso.rstr.length() << ">" << QString::fromStdString(qso.rstr) ;
            }
            if (!qso.rsts.empty()) {
                out << "<RST_SENT:" << qso.rsts.length() << ">" << QString::fromStdString(qso.rsts) ;
            }


            if (!qso.name.empty()) {
                out << "<NAME:" << qso.name.length() << ">" << QString::fromStdString(qso.name) ;
            }
            if (!qso.qth.empty()) {
                out << "<QTH:" << qso.qth.length() << ">" << QString::fromStdString(qso.qth) ;
            }

            out << "<APP_QSOLOGGER_TIMESTAMP:"
                << QString::fromStdString(qso.date).replace("-", "")
                << QString::fromStdString(qso.date).replace("-", "").length()
                << ">"
                << QString::fromStdString(qso.time).replace(":", "") ;

            out << "<EOR>\n";
        }
    }

    file.close();

    QMessageBox::information(this, "Экспорт в ADIF",
                             QString("Данные успешно экспортированы в ADIF файл: %1\nВсего QSO: %2")
                                 .arg(fileName).arg(QSOHystory.size()));
}


void MainWindow::on_ExportAdif_2_triggered()
{
    ExportToADIF();
}

void MainWindow::saveLogToFile(QString fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    for (const TQSO& qso : QSOHystory) {
        out << QString::fromStdString(qso.date) << ";" << QString::fromStdString(qso.time) << ";"
            << QString::fromStdString(qso.callsign) << ";" << QString::fromStdString(qso.band) << ";"
            << QString::fromStdString(qso.mode) << ";" << QString::fromStdString(qso.rstr) << ";"
            << QString::fromStdString(qso.rsts) << ";" << QString::fromStdString(qso.name) << ";"
            << QString::fromStdString(qso.qth) << "\n";
    }
    file.close();
}

void MainWindow::loadLogFromFile(QString fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(";");
        if (parts.size() < 9) continue;

        TQSO qso;
        qso.date = parts[0].toStdString();
        qso.time = parts[1].toStdString();
        qso.callsign = parts[2].toStdString();
        qso.band = parts[3].toStdString();
        qso.mode = parts[4].toStdString();
        qso.rstr = parts[5].toStdString();
        qso.rsts = parts[6].toStdString();
        qso.name = parts[7].toStdString();
        qso.qth = parts[8].toStdString();

        QSOHystory.append(qso);

        int row = ui->QSOTable->rowCount();
        ui->QSOTable->insertRow(row);
        for(int i = 0; i < 9; ++i) {
            ui->QSOTable->setItem(row, i, new QTableWidgetItem(parts[i]));
        }
    }
    rowcount = ui->QSOTable->rowCount();
    file.close();
}

void MainWindow::closeEvent(QCloseEvent *event) {


    if (QSOHystory.isEmpty()) { event->accept(); return; }

    auto resBtn = QMessageBox::question(this, "QSO Logger", "Сохранить лог?",
                                        QMessageBox::No | QMessageBox::Yes | QMessageBox::Cancel);
    if (resBtn == QMessageBox::Yes) {
        saveLogToFile(logFileName);
        event->accept();
    } else if (resBtn == QMessageBox::No) {
        event->accept();
    } else {
        event->ignore();
    }
}




void MainWindow::on_FilterDate_clicked()
{
    if (!isFiltered) {
        originalData = QSOHystory;
    }

    QDate startDate = ui->dateEditStart->date();
    QDate endDate = ui->dateEditEnd->date();

    QVector<TQSO> filteredData;

    for (const TQSO& qso : originalData) {

        QDate qsoDate = QDate::fromString(QString::fromStdString(qso.date), "yyyy-MM-dd");

        if (qsoDate >= startDate && qsoDate <= endDate) {
            filteredData.append(qso);
        }
    }

    QSOHystory = filteredData;
    isFiltered = true;
    Array2Table();
}


void MainWindow::on_ClearFilter_clicked()
{
    if (isFiltered) {
        QSOHystory = originalData;
        isFiltered = false;
        Array2Table();
    }
}

