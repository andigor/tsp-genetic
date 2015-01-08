#-------------------------------------------------
#
# Project created by QtCreator 2015-01-04T12:47:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    points.cpp \
    drawframe.cpp \
    paths.cpp \
    tools.cpp \
    graph.cpp

HEADERS  += widget.h \
    points.h \
    drawframe.h \
    paths.h \
    tools.h \
    graph.h

FORMS    += widget.ui

QMAKE_CXXFLAGS += -std=c++1y -Werror
