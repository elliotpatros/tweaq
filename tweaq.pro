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

HEADERS  += \
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

FORMS    += mainwindow.ui

ICON = Icons/tweaq_icon.icns

DEFINES  += __QTWEAQ_IS_DEFINED

QMAKE_CXXFLAGS += -Wall

DISTFILES += \
    License

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/StaticLibs/libsndfile/lib/release/ -lsndfile
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/StaticLibs/libsndfile/lib/debug/ -lsndfile
else:unix: LIBS += -L$$PWD/StaticLibs/libsndfile/lib/ -lsndfile

INCLUDEPATH += $$PWD/StaticLibs/libsndfile/include
DEPENDPATH += $$PWD/StaticLibs/libsndfile/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/StaticLibs/libsndfile/lib/release/libsndfile.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/StaticLibs/libsndfile/lib/debug/libsndfile.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/StaticLibs/libsndfile/lib/release/sndfile.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/StaticLibs/libsndfile/lib/debug/sndfile.lib
else:unix: PRE_TARGETDEPS += $$PWD/StaticLibs/libsndfile/lib/libsndfile.a
