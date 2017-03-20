#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T16:47:30
#
#-------------------------------------------------

QT       += core gui quickwidgets concurrent opengl

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = Pdp_Velos
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    model.cpp \
    station.cpp \
    trip.cpp \
    datafilereader.cpp \
    xmldatafilereader.cpp \
    csvdatafilereader.cpp \
    timelinewidget.cpp \
    matrixglwidget.cpp \
    mapglwidget.cpp \
    tripsfilter.cpp \
    stationsfilter.cpp \
    tripsselector.cpp \
    renderer.cpp \
    stationrenderer.cpp \
    stationssorter.cpp \
    triprenderer.cpp \
    glyphrenderer.cpp \
    selectorrenderer.cpp

HEADERS  += mainwindow.h \
    model.h \
    station.h \
    trip.h \
    datafilereader.h \
    xmldatafilereader.h \
    csvdatafilereader.h \
    timelinewidget.h \
    matrixglwidget.h \
    mapglwidget.h \
    tripsfilter.h \
    stationsfilter.h \
    tripsselector.h \
    renderer.h \
    stationrenderer.h \
    stationssorter.h \
    stationrenderer.h \
    triprenderer.h \
    glyphrenderer.h \
    selectorrenderer.h \
    typedefs.h \
    config.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    resources.qrc

CONFIG += C++14
QMAKE_CXXFLAGS_RELEASE +=
QMAKE_CXXFLAGS_DEBUG += -pg -Wunused-parameter -Wunused-variable -Wunused-value -Wunused
