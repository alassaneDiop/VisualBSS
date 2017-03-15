#include "mainwindow.h"
#include <QApplication>

#include <QSurfaceFormat>


static void setDefaultOpenGLSurfaceFormat()
{
  QSurfaceFormat format;
  format.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(format);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    setDefaultOpenGLSurfaceFormat();
    MainWindow w;
    w.show();
    return a.exec();
}
