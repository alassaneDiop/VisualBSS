#include "mainwindow.h"
#include <QApplication>

#include "model.h"

#include "qdebug.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Model model;
    MainWindow w(0, &model);

    w.show();
    return a.exec();
}
