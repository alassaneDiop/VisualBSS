#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T16:47:30
#
#-------------------------------------------------

QT       += core gui quickwidgets concurrent opengl

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = VisualBSS
TEMPLATE = app

INCLUDEPATH += $$PWD/include

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/station.cpp \
    src/trip.cpp \
    src/xmldatafilereader.cpp \
    src/csvdatafilereader.cpp \
    src/timelinewidget.cpp \
    src/matrixglwidget.cpp \
    src/mapglwidget.cpp \
    src/tripsfilter.cpp \
    src/stationsfilter.cpp \
    src/tripsselector.cpp \
    src/renderer.cpp \
    src/stationrenderer.cpp \
    src/stationssorter.cpp \
    src/triprenderer.cpp \
    src/glyphrenderer.cpp \
    src/selectorrenderer.cpp \
    src/abstractdatafilereader.cpp \
    src/datafilereader.cpp \
    src/stationsverticesbuilder.cpp \
    src/tripsverticesbuilder.cpp \
    src/glyphsverticesbuilder.cpp \
    src/data.cpp

HEADERS += abstractdatafilereader.h \
    config.h \
    csvdatafilereader.h \
    data.h \
    datafilereader.h \
    glyphrenderer.h \
    glyphsverticesbuilder.h \
    mainwindow.h \
    mapglwidget.h \
    matrixglwidget.h \
    renderer.h \
    selectorrenderer.h \
    station.h \
    stationrenderer.h \
    stationsfilter.h \
    stationssorter.h \
    stationsverticesbuilder.h \
    timelinewidget.h \
    trip.h \
    triprenderer.h \
    tripsfilter.h \
    tripsselector.h \
    tripsverticesbuilder.h \
    xmldatafilereader.h \

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    resources.qrc

CONFIG += C++14
QMAKE_CXXFLAGS_RELEASE += -O2
QMAKE_CXXFLAGS_DEBUG += -pg -Wunused-parameter -Wunused-variable -Wunused-value -Wunused
