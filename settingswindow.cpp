#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "Operator.h"

TOperator Operator;
FILE *foperator;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_MyCS_returnPressed()
{

}


void SettingsWindow::on_buttonBox_accepted()
{
    QString callsign = ui->MyCS->text().toUpper();
    const char* callsign_str = callsign.toLocal8Bit().data();

    QString name = ui->MyName->text();
    const char* name_str = name.toLocal8Bit().data();

    QString city = ui->MyCity->text();
    const char* city_str = city.toLocal8Bit().data();

    QString cq = ui->MyCQ->text();
    const char* cq_str = cq.toLocal8Bit().data();

    QString itu = ui->MyITU->text();
    const char* itu_str = itu.toLocal8Bit().data();

    QString qth = ui->MyQTH->text();
    const char* qth_str = qth.toLocal8Bit().data();

    strcpy(Operator.callsign, callsign_str);
    strcpy(Operator.name, name_str);
    strcpy(Operator.city, city_str);
    strcpy(Operator.cq, cq_str);
    strcpy(Operator.itu, itu_str);
    strcpy(Operator.qthloc, qth_str);

    foperator = fopen("C:\\Users\\Mikhail\\Documents\\LogBook\\Operator.txt", "w");
    fprintf(foperator, "CALLSILN: %s", Operator.callsign);
    fprintf
    fclose(foperator);

    qDebug(Operator.callsign);
}

