CONFIG -= app_bundle
QT += widgets

unix {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += mpv
}

win32 {
    INCLUDEPATH += "$$PWD/mpv/include"
    LIBS += -L"$$PWD/mpv/win/x86_64" -llibmpv
    LIBS += -lopengl32
}

HEADERS = \
    mainwindow.h \
    sliderstyle.h \
    exifparser.h \
    compositor.h \
    mpvinterface.h \
    autohidewidget.h \
    cursormanager.h \
    qthelper.h
SOURCES = main.cpp \
    mainwindow.cpp \
    exifparser.cpp \
    compositor.cpp \
    mpvinterface.cpp \
    autohidewidget.cpp \
    cursormanager.cpp

FORMS += \
    mainwindow.ui
