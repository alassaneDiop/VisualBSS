#include "mainwindow.h"
#include <QApplication>
#include <bandit/bandit.h>
#include <QSurfaceFormat>

static void setDefaultOpenGLSurfaceFormat()
{
  QSurfaceFormat format;
#ifndef Q_OS_LINUX
  format.setVersion(3, 3);
#endif
  format.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(format);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    setDefaultOpenGLSurfaceFormat();
    MainWindow w;
    w.showMaximized();
    return a.exec(); //& bandit::run(argc, argv);
}
