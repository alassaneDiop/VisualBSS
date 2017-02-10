#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T16:47:30
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pdp_Velos
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    model.cpp \
    filter.cpp \
    station.cpp \
    trip.cpp \
    datafilereader.cpp \
    xmldatafilereader.cpp \
    csvdatafilereader.cpp

HEADERS  += mainwindow.h \
    model.h \
    filter.h \
    station.h \
    trip.h \
    datafilereader.h \
    xmldatafilereader.h \
    csvdatafilereader.h

FORMS    += mainwindow.ui
