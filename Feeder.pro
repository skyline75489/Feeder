#-------------------------------------------------
#
# Project created by QtCreator 2014-02-25T16:41:09
#
#-------------------------------------------------

QT       += core gui network webkitwidgets script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Feeder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    o2/src/o1.cpp \
    o2/src/o1requestor.cpp \
    o2/src/o2.cpp \
    o2/src/o2facebook.cpp \
    o2/src/o2gft.cpp \
    o2/src/o2reply.cpp \
    o2/src/o2replyserver.cpp \
    o2/src/o2requestor.cpp \
    o2/src/o2settingsstore.cpp \
    o2/src/o2skydrive.cpp \
    o2/src/oxtwitter.cpp \
    o2/src/simplecrypt.cpp \
    subscribedialog.cpp

HEADERS  += mainwindow.h \
    o2/src/o1.h \
    o2/src/o1dropbox.h \
    o2/src/o1flickr.h \
    o2/src/o1requestor.h \
    o2/src/o1twitter.h \
    o2/src/o2.h \
    o2/src/o2abstractstore.h \
    o2/src/o2facebook.h \
    o2/src/o2gft.h \
    o2/src/o2globals.h \
    o2/src/o2reply.h \
    o2/src/o2replyserver.h \
    o2/src/o2requestor.h \
    o2/src/o2settingsstore.h \
    o2/src/o2skydrive.h \
    o2/src/oxtwitter.h \
    o2/src/simplecrypt.h \
    subscribedialog.h

FORMS    += mainwindow.ui \
    subscribedialog.ui

OTHER_FILES += \
    o2/src/src.pri
