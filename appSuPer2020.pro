#-------------------------------------------------
#
# Project created by QtCreator 2019-12-03T19:01:24
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = appSuPer2020
TEMPLATE = app


SOURCES += main.cpp\
        cihm.cpp \
    cbadge.cpp \
    clabelbadge.cpp \
    clecteurrfid.cpp \
    cbdd.cpp \
    cserver.cpp \
    cthread.cpp \
    cclientconnection.cpp \
    ctcpsocket.cpp \
    creader.cpp \
    capp.cpp \
    clabellecteurrfid.cpp

HEADERS  += cihm.h \
    config.h \
    cbadge.h \
    clabelbadge.h \
    clecteurrfid.h \
    cbdd.h \
    cserver.h \
    cthread.h \
    cclientconnection.h \
    ctcpsocket.h \
    creader.h \
    capp.h \
    clabellecteurrfid.h

FORMS    += cihm.ui
