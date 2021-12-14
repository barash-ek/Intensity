QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += \
    c++11 \
    precompile_header

PRECOMPILED_HEADER = pch.h

HEADERS += \
    image.h \
    imagearea.h \
    imagewidget.h \
    mainwindow.h  \
    pch.h \
    rightbar.h

SOURCES += \
    image.cpp \
    imagearea.cpp \
    imagewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    rightbar.cpp
