#include "mainwindow.h"
#include <QApplication>

#include "model.h"

#include "qdebug.h"

int main(int argc, char *argv[])
{
    Model model;
    const QString filename("2014-01 - Citi Bike trip data.csv");

    if (model.loadData(filename))
         qDebug() << QString::number(model.getTrips().count());
       /* for (const auto& trip : model)
            qDebug((QString(trip) + '\n').toStdString().c_str());*/

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
