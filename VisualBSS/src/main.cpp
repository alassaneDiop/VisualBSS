/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include "mainwindow.h"
#include <QApplication>
#include <bandit/bandit.h>
#include <QSurfaceFormat>
#include "trip.h"
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

