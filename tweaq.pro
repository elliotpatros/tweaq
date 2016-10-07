#-------------------------------------------------
#
# Project created by QtCreator 2016-09-23T12:06:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tweaq
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    tqtreeview.cpp \
    m_tweaq.cpp \
    tqlineedit.cpp \
    instancemeta.cpp \
    tqcombobox.cpp \
    tqfileinfo.cpp \
    libsf.cpp \
    afmeta.cpp \
    afitem.cpp \
    afmodel.cpp \
    abstractfileimporter.cpp \
    afimporter.cpp \
    libmeta.cpp \
    libmodel.cpp \
    libimporter.cpp \
    parameterhandle.cpp \
    parameterdialog.cpp

HEADERS += \
    mainwindow.h \
    tqtreeview.h \
    m_tweaq.h \
    tqlineedit.h \
    instancemeta.h \
    tqcombobox.h \
    tqfileinfo.h \
    libsf.h \
    afmeta.h \
    afitem.h \
    afmodel.h \
    abstractfileimporter.h \
    afimporter.h \
    libmeta.h \
    libmodel.h \
    libimporter.h \
    parameterhandle.h \
    qtweaq.h \
    parameterdialog.h

FORMS += mainwindow.ui

ICON = Icons/tweaq_icon.icns

DEFINES += __QTWEAQ_IS_DEFINED

QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -Weffc++ -Wold-style-cast \
-Wdelete-non-virtual-dtor -Wnon-virtual-dtor -Winit-self -Wunreachable-code

DISTFILES += \
    License

INCLUDEPATH += $$PWD/StaticLibs/libsndfile/include
DEPENDPATH += $$PWD/StaticLibs/libsndfile/include

LIBS += -L$$PWD/StaticLibs/libsndfile/lib/ -lsndfile
PRE_TARGETDEPS += $$PWD/StaticLibs/libsndfile/lib/libsndfile.a
