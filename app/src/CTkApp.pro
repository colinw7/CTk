TEMPLATE = lib

TARGET = CTkApp

DEPENDPATH += .

QT += widgets

CONFIG += staticlib
CONFIG += debug

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQUTIL_PEN \
-DCQUTIL_BRUSH \
-DCQUTIL_FONT \
-DCQUTIL_IMAGE \
-DCBRUSH_IMAGE \

MOC_DIR = .moc

# Input
SOURCES += \
CTkAppCommands.cpp \
CTkApp.cpp \
CTkAppGridLayout.cpp \
CTkAppImage.cpp \
CTkAppLayoutWidget.cpp \
CTkAppOptData.cpp \
CTkAppPackLayout.cpp \
CTkAppPlaceLayout.cpp \
CTkAppWidget.cpp \

HEADERS += \
../include/CTkAppCommands.h \
../include/CTkApp.h \
../include/CTkAppImage.h \
../include/CTkAppLayout.h \
../include/CTkAppOptData.h \
../include/CTkAppOptionValue.h \
../include/CTkAppWidget.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../../CQUtil/include \
../../../CTclApp/include \
../../../CImageLib/include \
../../../CRGBName/include \
../../../CFont/include \
../../../CFile/include \
../../../CMath/include \
../../../CStrUtil/include \
../../../CUtil/include \
../../../COS/include \
/usr/include/tcl \
