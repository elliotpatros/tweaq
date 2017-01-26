QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tweaq2
TEMPLATE = app

CONFIG += debug

SOURCES += main.cpp\
    mainwindow.cpp \
    treeviewaudiofile.cpp \
    lineeditexportto.cpp \
    comboboxtweaq.cpp \
    libsf.cpp \
    af_properties.cpp \
    af_item.cpp \
    af_model.cpp \
    fileimporter.cpp \
    comboboxdsp.cpp \
    comboboxremove.cpp \
    tweaqapi.cpp \
    parameterdialog.cpp \
    parameterdialoglayout.cpp \
    parameterhandle.cpp \
    externalinterface.cpp \
    externalinterfacemanager.cpp \
    backgroundworker.cpp

HEADERS  += mainwindow.h \
    tweaq.h \
    treeviewaudiofile.h \
    lineeditexportto.h \
    comboboxtweaq.h \
    libsf.h \
    af_properties.h \
    af_item.h \
    af_model.h \
    fileimporter.h \
    comboboxdsp.h \
    comboboxremove.h \
    mainwindowhelper.h \
    tweaqapi.h \
    parameterdialog.h \
    parameterdialoglayout.h \
    parameterhandle.h \
    externalinterface.h \
    externalinterfacemanager.h \
    backgroundworker.h

APP_QML_FILES.path  = Contents/Resources/extern
APP_QML_FILES.files = \
    $$PWD/extern/libchange_gain.so \
    $$PWD/extern/libmix_to_mono.so \
    $$PWD/extern/libnormalize.so \
    $$PWD/extern/libdeinterleave.so \
    $$PWD/extern/libfade_in.so

QMAKE_BUNDLE_DATA += APP_QML_FILES

FORMS    += mainwindow.ui

ICON = icons/tweaq_icon.icns

DISTFILES += License

macx: LIBS += -L$$PWD/staticlibs/libsndfile/lib/ -lsndfile.1

# release flags
CONFIG(release, debug|release) {
QMAKE_CXXFLAGS += -Wpointer-arith -fno-exceptions -O2 -flto
macx: LIBS += -Wpointer-arith -dead_strip -fno-exceptions -O2 -flto
}

INCLUDEPATH += $$PWD/staticlibs/libsndfile/include
DEPENDPATH += $$PWD/staticlibs/libsndfile/include
