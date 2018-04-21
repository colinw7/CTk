TEMPLATE = lib

TARGET = CTk

DEPENDPATH += .

QT += widgets

CONFIG += staticlib
CONFIG += debug

QMAKE_CXXFLAGS += -std=c++14

MOC_DIR = .moc

# Input
SOURCES += \
CTk.cpp \
CTkGridLayout.cpp \
CTkLayoutWidget.cpp \
CTkPackLayout.cpp \
CTkPlaceLayout.cpp \

HEADERS += \
../include/CTk.h \
../include/CTkLayout.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../CQUtil/include \
../../CTcl/include \
../../CImageLib/include \
../../CRGBName/include \
../../CFont/include \
../../CFile/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
