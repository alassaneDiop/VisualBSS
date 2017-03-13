#include "mainwindow.h"
#include <QApplication>

#include <QSurfaceFormat>


static void setDefaultOpenGLSurfaceFormat()
{
  QSurfaceFormat format;
//  format.setVersion(4, 5);
  format.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(format);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    setDefaultOpenGLSurfaceFormat();
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
