QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tweaq
TEMPLATE = app

CONFIG += debug

SOURCES += main.cpp\
    mainwindow.cpp \
    treeviewaudiofile.cpp \
    lineeditexportto.cpp \
    libsf.cpp \
    af_properties.cpp \
    af_item.cpp \
    af_model.cpp \
    comboboxdsp.cpp \
    comboboxremove.cpp \
    tweaqapi.cpp \
    parameterdialog.cpp \
    parameterdialoglayout.cpp \
    parameterhandle.cpp \
    externalinterface.cpp \
    externalinterfacemanager.cpp \
    backgroundworker.cpp \
    progressdialog.cpp \
    combobox.cpp \
    fileimporter.cpp

HEADERS  += mainwindow.h \
    tweaq.h \
    treeviewaudiofile.h \
    lineeditexportto.h \
    libsf.h \
    af_properties.h \
    af_item.h \
    af_model.h \
    comboboxdsp.h \
    comboboxremove.h \
    mainwindowhelper.h \
    tweaqapi.h \
    parameterdialog.h \
    parameterdialoglayout.h \
    parameterhandle.h \
    externalinterface.h \
    externalinterfacemanager.h \
    backgroundworker.h \
    progressdialog.h \
    combobox.h \
    fileimporter.h

APP_QML_FILES.path  = Contents/Resources/extern
APP_QML_FILES.files = $$files($$PWD/extern/*.so, false)

QMAKE_BUNDLE_DATA += APP_QML_FILES

FORMS += mainwindow.ui

ICON = icons/tweaq_icon.icns

DISTFILES += License

QMAKE_CXXFLAGS += -W -Wall -Wpedantic

# release flags
CONFIG(release, debug|release) {
QMAKE_CXXFLAGS += -Wpointer-arith -fno-exceptions -O2 -flto
macx: LIBS += -Wpointer-arith -dead_strip -fno-exceptions -O2 -flto
}

# libsndfile
macx: LIBS += -L/usr/local/Cellar/libsndfile/1.0.26/lib/ -lsndfile.1
INCLUDEPATH += /usr/local/Cellar/libsndfile/1.0.26/include
DEPENDPATH += /usr/local/Cellar/libsndfile/1.0.26/include

# libsamplerate
macx: LIBS += -L/usr/local/Cellar/libsamplerate/0.1.9/lib/ -lsamplerate.0
INCLUDEPATH += /usr/local/Cellar/libsamplerate/0.1.9/include
DEPENDPATH += /usr/local/Cellar/libsamplerate/0.1.9/include
