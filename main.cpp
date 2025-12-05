#include "mainwindow.h"
#include "QSO.h"

#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // C++ пример

    MainWindow w;
    w.show();
    return a.exec();
}
