QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AlgoPosePlot
TEMPLATE = app

CONFIG += c++11

# Output directory
DESTDIR = ../../build/Desktop_Qt_6_10_0-Debug

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    posfilereader.cpp \
    graphwidget.cpp

HEADERS += \
    mainwindow.h \
    posfilereader.h \
    graphwidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
