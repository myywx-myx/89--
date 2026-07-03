QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = flashcard
TEMPLATE = app

# 源文件
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    card.cpp \
    deck.cpp \
    datamanager.cpp \
    reviewscheduler.cpp

# 头文件
HEADERS += \
    mainwindow.h \
    card.h \
    deck.h \
    datamanager.h \
    reviewscheduler.h

# 包含路径（可选，通常不需要）
# INCLUDEPATH += $$PWD