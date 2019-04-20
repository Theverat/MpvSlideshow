CONFIG -= app_bundle
QT += widgets

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig debug
PKGCONFIG += mpv

HEADERS = \
    mainwindow.h \
    sliderstyle.h \
    exifparser.h \
    compositor.h \
    mpvinterface.h \
    autohidewidget.h
SOURCES = main.cpp \
    mainwindow.cpp \
    exifparser.cpp \
    compositor.cpp \
    mpvinterface.cpp \
    autohidewidget.cpp

FORMS += \
    mainwindow.ui
