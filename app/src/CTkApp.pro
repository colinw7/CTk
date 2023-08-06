TEMPLATE = lib

TARGET = CTkApp

DEPENDPATH += .

QT += widgets svg

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
CTkApp.cpp \
CTkAppCommands.cpp \
CTkAppGridLayout.cpp \
CTkAppImage.cpp \
CTkAppFont.cpp \
CTkAppLayoutWidget.cpp \
CTkAppOptData.cpp \
CTkAppPackLayout.cpp \
CTkAppPlaceLayout.cpp \
CTkAppWidget.cpp \
CTkAppUtil.cpp \
\
CTclApp.cpp \
CTclAppCommand.cpp \
\
CQLabelImage.cpp \
CQSpinList.cpp \
CQSlider.cpp \
CSVGUtil.cpp \
CQRotatedText.cpp \

HEADERS += \
../include/CTkApp.h \
../include/CTkAppCommands.h \
../include/CTkAppEventData.h \
../include/CTkAppGridLayout.h \
../include/CTkAppImage.h \
../include/CTkAppFont.h \
../include/CTkAppLayoutWidget.h \
../include/CTkAppOptData.h \
../include/CTkAppOptionValue.h \
../include/CTkAppPackLayout.h \
../include/CTkAppPlaceLayout.h \
../include/CTkAppLayout.h \
../include/CTkAppWidget.h \
../include/CTkAppUtil.h \
\
../include/CTclAppCommand.h \
../include/CTclApp.h \
\
CQLabelImage.h \
CQSpinList.h \
CQSlider.h \
CSVGUtil.h \
CQRotatedText.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../../CQUtil/include \
../../../CTclApp/include \
../../../CXLib/include \
../../../CReadLine/include \
../../../CImageLib/include \
../../../CRGBName/include \
../../../CFont/include \
../../../CFile/include \
../../../CMath/include \
../../../CStrUtil/include \
../../../CGlob/include \
../../../CUtil/include \
../../../COS/include \
/usr/include/tcl \
