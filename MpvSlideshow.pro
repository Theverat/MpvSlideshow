CONFIG -= app_bundle
QT += widgets

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig debug
PKGCONFIG += mpv

HEADERS = \
    mpvwidget.h \
    mainwindow.h \
    slideshow.h \
    sliderstyle.h
SOURCES = main.cpp \
    mpvwidget.cpp \
    mainwindow.cpp \
    slideshow.cpp

FORMS += \
    mainwindow.ui